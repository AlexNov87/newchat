#include "srv.h"

std::string MainServer::LoginUser(shared_task action, shared_flatbuf buffer, shared_stream stream)
{
    try
    {
        const auto &name = action->at(CONSTANTS::LF_NAME);
        const auto &pass = action->at(CONSTANTS::LF_PASSWORD);
        // ЕСЛИ АВТОРИЗОВАН
        if (!IsAutorizatedUser(name, pass))
        {
            return ServiceChatroomServer::MakeAnswerError("YOU ARE NOT AUTHORIZED", __func__, CONSTANTS::ACT_LOGIN);
        }
        // Генерируем токен
        std::string token = tokezier_.GenerateHEXToken();

        {  
            // ЕСЛИ ЕСТЬ ТАКАЯ КОМНАТА
            std::string &roomname = action->at(CONSTANTS::LF_ROOMNAME);
            
            //Во время добавления юзера с комнатами ничего не должнго происходить... 
            std::unique_lock<std::mutex> ul(mtx_lock_user_operations_);
            if (!rooms_.count(roomname) > 0)
            {
                return ServiceChatroomServer::MakeAnswerError("NO ROOM: " + roomname, __func__, CONSTANTS::ACT_LOGIN);
            };
            auto room = rooms_.at(roomname);           
            room->AddUser(stream, buffer, name, token);
            return "";
        } // конец блокировки
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError("FAILED TO ADD USER", __func__, CONSTANTS::ACT_LOGIN);
    }
}

std::string MainServer::CreateRoom(std::string room)
{
    try
    {
         // Во время получения с комнатами ничего не должно происходить.
        std::lock_guard<std::mutex> lg(mtx_lock_user_operations_);
        
        if (rooms_.contains(room))
        {
            return ServiceChatroomServer::MakeAnswerError("FAILED TO CREATE ROOM, ROOM WITH THIS NAME IS EXISTS", __func__, CONSTANTS::ACT_CREATE_ROOM);
        }
        rooms_.insert({room, std::make_shared<Chatroom>(&*this, room)});
        return ServiceChatroomServer::Srv_MakeSuccessCreateRoom(room);
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError("FAILED TO CREATE ROOM", __func__, CONSTANTS::ACT_CREATE_ROOM);
    }
}

std::string MainServer::GetRoomUsersList(const std::string &roomname)
{
    try
    {
        // Во время получения с комнатами ничего не должно происходить.
        std::lock_guard<std::mutex> lg(mtx_lock_user_operations_);
        
        if (!rooms_.contains(roomname))
        {
            return ServiceChatroomServer::MakeAnswerError("THERE IS NO ROOM: " + roomname, __func__, CONSTANTS::ACT_GET_USERS);
        }
        std::string userlist = rooms_.at(roomname)->RoomMembers();
        return ServiceChatroomServer::Srv_MakeSuccessGetUsers(std::move(userlist));
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError(ex.what(), __func__, CONSTANTS::ACT_GET_USERS);
    }
}

std::string MainServer::GetRoomsList()
{
    try
    {
        // Во время получения с комнатами ничего не должно происходить.
        std::lock_guard<std::mutex> lg(mtx_lock_user_operations_);
    
        std::stringstream strm;
        int rms = static_cast<int>(rooms_.size());
        int cnt = 0;
        strm << "[ ";
        {
            for (auto &&room : rooms_)
            {
                strm << '"' << room.first << '"';
                ++cnt;
                if (cnt >= rms)
                {
                    break;
                }
                strm << " , ";
            }
        }
        strm << " ]";
        return ServiceChatroomServer::Srv_MakeSuccessRoomList(std::move(strm.str()));
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError(ex.what(), __func__, CONSTANTS::ACT_ROOM_LIST);
    }
}

std::string MainServer::AddUserToSQL(const std::string &name, const std::string &passhash)
{
    std::lock_guard<std::mutex> lg(mtx_lock_sql_operations_);
    try
    {
        // ЕСЛИ УЖЕ ЗАРЕГИСТРИРОВАН
        if (AlreadyUserRegistered(name))
        {
            return ServiceChatroomServer::MakeAnswerError("USER WITH THIS NICK ALREADY REGISTERED", __func__, CONSTANTS::ACT_CREATE_USER);
        };
        /*


        ЛОГИКА ЗАПРОСА К SQL И ДОБАВЛЕНИЕ В ОБЩИЙ СПИСОК??






        */

        return ServiceChatroomServer::Srv_MakeSuccessCreateUser(name);
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError(ex.what(), __func__, CONSTANTS::ACT_CREATE_USER);
    }
}
