#include "service.h"

namespace ServiceChatroomServer
{

    std::optional<std::string> CHK_ServerLoadObject(const boost::json::value &obj)
    {
        if (!obj.is_object())
        {
            return "OBJECT TYPE IS INCORRECT, JSON::OBJECT";
        }
        auto &js = obj.as_object();

        if (!js.contains(CONSTANTS::IP))
        {
            return "IP FIELD IS MISSING";
        }
        if (!js.at(CONSTANTS::IP).is_string())
        {
            return "IP MUST BE STRING";
        }

        if (!js.contains(CONSTANTS::PORT))
        {
            return "PORT FIELD IS MISSING";
        }
        if (!js.at(CONSTANTS::PORT).is_int64())
        {
            return "PORT MUST BE INT";
        }
        if (js.at(CONSTANTS::PORT).as_int64() < 0 || js.at(CONSTANTS::PORT).as_int64() > 65535)
        {
            return std::to_string(js.at(CONSTANTS::PORT).as_int64()) + " IS INCORRECT PORT VALUE";
        }

        if (!js.contains(CONSTANTS::CHATROOMS))
        {
            return std::nullopt;
        }

        auto &chrooms = js.at(CONSTANTS::CHATROOMS);
        if (!chrooms.is_array())
        {
            return "CHATROOM IS NOT ARRAY";
        }

        for (auto &&chr : chrooms.as_array())
        {
            if (!chr.is_string())
            {
                return "CHECK ROOMS NAMES";
            }
        }
        return std::nullopt;
    };
}
// ПРОВЕРКА ОБЩАЯ
namespace ServiceChatroomServer
{
    // ПРОВЕРЯЕТ ЕСТЬ ЛИ ПОЛЕ И ПУСТОЕ ЛИ ОНО
    std::optional<std::string> CHK_OneFieldExistsAndNotEmpty(const task &action, const std::string &fieldname)
    {
        if (!action.contains(fieldname))
        {
            return "NO ACTION " + fieldname;
        }
        if (action.at(CONSTANTS::LF_ACTION).empty())
        {
            return "EMPTY " + fieldname;
        }

        return std::nullopt;
    }

    std::optional<std::string> CHK_FieldActionIncorrect(const task &action)
    {

        auto reason = CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_ACTION);
        if (reason)
        {
            return *reason;
        }

        if (!Service::Additional::chatroom_actions.contains(action.at(CONSTANTS::LF_ACTION)) && !Service::Additional::server_actions.contains(action.at(CONSTANTS::LF_ACTION)))
        {
            return "ACTION: " + action.at(CONSTANTS::LF_ACTION) + " IS UNRECOGNIZED";
        }
        return std::nullopt;
    }

    ///@brief Проверяет валидность направления
    std::optional<std::string> CHK_FieldDirectionIncorrect(const task &action)
    {

        auto reason = CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_DIRECTION);
        if (reason)
        {
            return *reason;
        }

        const auto &chk = action.at(CONSTANTS::LF_DIRECTION);
        if (chk != CONSTANTS::RF_DIRECTION_CHATROOM && chk != CONSTANTS::RF_DIRECTION_SERVER)
        {
            return "THE DIRECTION IS NOT RECOGNIZED";
        }

        return std::nullopt;
    }

    ///@brief Проверяет валидность токена
    std::optional<std::string> CHK_FieldTokenIncorrect(const task &action)
    {
        auto reason = CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_TOKEN);
        if (reason)
        {
            return *reason;
        }

        if (action.at(CONSTANTS::LF_TOKEN).size() != CONSTANTS::N_TOKEN_LEN)
        {
            std::cout << action.at(CONSTANTS::LF_TOKEN).size() << " != " << CONSTANTS::N_TOKEN_LEN << '\n';
            return "SIZE OF TOKEN: " + std::to_string(action.at(CONSTANTS::LF_TOKEN).size()) + " IS INCORRECT";
        }
        return std::nullopt;
    }

}

// ПРОВЕРКА ЗАПРОСА К ЧАТРУМУ
namespace ServiceChatroomServer
{

    ///@brief Проверяет валидность контейнера действия послания сообщения
    std::optional<std::string> Chr_ActionSendMessageIncorrect(const task &action)
    {
        auto reason = CHK_FieldTokenIncorrect(action);
        if (reason)
        {
            return *reason;
        }

        reason = CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_MESSAGE);
        if (reason)
        {
            return *reason;
        }

        if (action.at(CONSTANTS::LF_MESSAGE).size() > CONSTANTS::N_MAX_MESSAGE_LEN)
        {
            return "TOO LARGE MESSAGE";
        }
        return std::nullopt;
    }

    ///@brief Проверяет валидность контейнера действия отключения
    std::optional<std::string> Chr_ActionDisconnectIncorrect(const task &action)
    {
        auto reason = CHK_FieldTokenIncorrect(action);

        if (reason)
        {
            return *reason;
        }
        return std::nullopt;
    }

    std::optional<std::string> CHK_Chr_CheckErrorsChatRoom(const task &action)
    {

        std::optional<std::string> reason;

        auto testcase = Service::Additional::action_scernario.at(action.at(CONSTANTS::LF_ACTION));
        switch (testcase)
        {
        case Service::ACTION::DISCONNECT:
        {
            reason = Chr_ActionDisconnectIncorrect(action);
        }
        break;

        case Service::ACTION::SEND_MESSAGE:
        {
            reason = Chr_ActionSendMessageIncorrect(action);
        }
        break;
        default:
            return action.at(CONSTANTS::LF_ACTION) + "CAN NOT BE A CHATROOM ACTION";
            break;
        }

        if (reason)
        {
            return *reason;
        }

        return std::nullopt;
    };
}

namespace ServiceChatroomServer
{
    // ЯВЛЯЕТСЯ ЛИ ЗАПРОС К СЕРВЕРУ
    std::optional<std::string> CHK_Srv_IsAddressedToServer(const task &action)
    {
        if (!(Service::Additional::server_actions.contains(action.at(CONSTANTS::LF_ACTION))))
        {
            return "ACTION " + action.at(CONSTANTS::LF_ACTION) + " CAN NOT BE ADDRESSED TO SERVER";
        }
        return std::nullopt;
    }
    // БАЗОВЫЕ ПРОВЕРКИ ЗАПРОСА К СЕРВЕРУ
    std::optional<std::string> CHK_Srv_BaseToServerCheckIncorrect(const task &action)
    {
        //    std::cout<< "BASESRVGOOD1ENTER\n";
        if (action.at(CONSTANTS::LF_DIRECTION) != CONSTANTS::RF_DIRECTION_SERVER)
        {
            return "DIRECTION TO SERVER" + action.at(CONSTANTS::LF_DIRECTION) + " IS INCORRECT";
        }
        std::optional<std::string> reason;
        // ПРОВЕРКА К СЕРВЕРУ ЛИ ОТНОСИТСЯ ДЕЙСТВИЕ
        reason = CHK_Srv_IsAddressedToServer(action);
        if (reason)
        {
            return *reason;
        }
        return std::nullopt;
    }

    // ПРОВЕРКА ЛОГИНА
    std::optional<std::string> CHK_Srv_ActionLoginIncorrect(const task &action)
    {
        // ПРОВЕРКА ПОЛЕЙ ТОКЕНА НЕ НУЖНА - СЕРВЕР ЕЩЕ НЕ ДАЛ ТОКЕН
         return CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_NAME, CONSTANTS::LF_ROOMNAME, CONSTANTS::LF_PASSWORD);
    }

    // ПРОВЕРКА ПОЛУЧЕНИЯ ПОЛЬЗОВАТЕЛЕЙ
    std::optional<std::string> CHK_Srv_ActionGetUsersIncorrect(const task &action)
    {
        return CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_ROOMNAME);
    }

    // ПРОВЕРКА СОЗДАНИЯ ПОЛЬЗОВАТЕЛЯ
    std::optional<std::string> CHK_Srv_ActionCreateUserIncorrect(const task &action)
    {
        return CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_NAME, CONSTANTS::LF_PASSWORD);
    }

    // ПРОВЕРКА СОЗДАНИЯ КОМНАТЫ
    std::optional<std::string> CHK_Srv_ActionCreateRoomIncorrect(const task &action)
    {
         return CHK_FieldExistsAndNotEmpty(action, CONSTANTS::LF_ROOMNAME);
    }

    std::optional<std::string> CHK_Chr_CheckErrorsChatServer(const task &action)
    {

        auto reason = CHK_Srv_BaseToServerCheckIncorrect(action);
        if (reason)
        {
            return *reason;
        }

        auto testcase = Service::Additional::action_scernario.at(action.at(CONSTANTS::LF_ACTION));
        switch (testcase)
        {
        case Service::ACTION::CREATE_ROOM:
            reason = CHK_Srv_ActionCreateRoomIncorrect(action);
            break;

        case Service::ACTION::CREATE_USER:
            reason = CHK_Srv_ActionCreateUserIncorrect(action);
            break;

        case Service::ACTION::GET_USERS:
            reason = CHK_Srv_ActionGetUsersIncorrect(action);
            break;

        case Service::ACTION::LOGIN:
            reason = CHK_Srv_ActionLoginIncorrect(action);
            break;
        default:
            break;
        }
        return reason;
    };

}
