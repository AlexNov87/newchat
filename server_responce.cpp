#include "service.h"
using namespace std;
namespace ServiceChatroomServer
{
    // ОТВЕТ СЕРВЕРА НА ОШИБКУ
    std::string MakeAnswerError(std::string reason, string initiator, std::string action)
    {
        unordered_map<string, string> res{
            {CONSTANTS::LF_RESULT, CONSTANTS::RF_ERROR},
            {CONSTANTS::LF_REASON, std::move(reason)},
            {CONSTANTS::LF_INITIATOR, std::move(initiator)},
            {CONSTANTS::LF_ACTION, std::move(action)}};
        return Service::SerializeUmap(res);
    };
}

// SUCESS CHATROOM
namespace ServiceChatroomServer
{
    task GetSuccess()
    {
        return {
            {CONSTANTS::LF_RESULT, CONSTANTS::RF_SUCCESS},
        };
    }

    // ОТВЕТ СЕРВЕРА  НА УСПЕШНОЕ ПОСЛАНИЕ СООБЩЕНИЯ
    std::string Chr_MakeSuccessSendMessage()
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_SEND_MESSAGE;
        return Service::SerializeUmap<std::string, std::string>(res);
    };

}

// SUCESS SERVER
namespace ServiceChatroomServer
{
    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ ПОЛУЧЕНИЕ ПОЛЬЗОВАТЕЛЕЙ
    std::string Srv_MakeSuccessGetUsers(std::string userlist)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_GET_USERS;
        res[CONSTANTS::LF_USERS] = userlist;
        return Service::SerializeUmap<std::string, std::string>(res);
    };

    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ
    std::string Srv_MakeSuccessLogin(std::string token, std::string roomname, std::string lstmsg)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_LOGIN;
        res[CONSTANTS::LF_TOKEN] = std::move(token);
        res[CONSTANTS::LF_ROOMNAME] = std::move(roomname);
        res[CONSTANTS::LF_LAST_MSG] = std::move(lstmsg);
        return Service::SerializeUmap<std::string, std::string>(res);
    };
    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ СОЗДАНИЕ(регистрацию) ПОЛЬЗОВАТЕЛЯ
    std::string Srv_MakeSuccessCreateUser(std::string name)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_CREATE_USER;
        res[CONSTANTS::LF_NAME] = std::move(name);
        return Service::SerializeUmap<std::string, std::string>(res);
    };
    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ СОЗДАНИЕ КОМНАТЫ
    std::string Srv_MakeSuccessCreateRoom(std::string name)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_CREATE_ROOM;
        res[CONSTANTS::LF_ROOMNAME] = std::move(name);

        return Service::SerializeUmap<std::string, std::string>(res);
    };
    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ ПОЛУЧЕНИЕ СПИСКА КОМНАТ
    std::string Srv_MakeSuccessRoomList(std::string roomlist)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_ROOM_LIST;
        res[CONSTANTS::LF_ROOMLIST] = std::move(roomlist);
        return Service::SerializeUmap<std::string, std::string>(res);
    };

    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ ПОЛУЧЕНИЕ ПРЕДЫДУЩИХ СООБЩЕНИЙ
    std::string Chr_MakeSuccessLastMessages(std::string msglist)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_EARLIER_MESS;
        res[CONSTANTS::LF_MESSAGE] = std::move(msglist);
        return Service::SerializeUmap<std::string, std::string>(res);
    };

    // ОТВЕТ СЕРВЕРА НА УСПЕШНОЕ ПОЛУЧЕНИЕ СООБЩЕНИЯ ЮЗЕРА
    std::string Chr_MakeSuccessUserMessage(std::string username, std::string msg)
    {
        task res = GetSuccess();
        res[CONSTANTS::LF_ACTION] = CONSTANTS::ACT_USER_MESSAGE;
        res[CONSTANTS::LF_NAME] = std::move(username);
        res[CONSTANTS::LF_MESSAGE] = std::move(msg);
        return Service::SerializeUmap<std::string, std::string>(res);
    };

}