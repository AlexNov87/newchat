#include "srv.h"

std::atomic_int ServerSession::exempslars_s = 0;
std::string ServerSession::ExecuteReadySession(shared_task action)
{
    try
    {
        Service::ACTION act = Service::Additional::action_scernario.at(action->at(CONSTANTS::LF_ACTION));
        switch (act)
        {
        case Service::ACTION::CREATE_ROOM:    
            ZyncPrint("CREATE_ROOM:");
            return server_->CreateRoom(std::move(action->at(CONSTANTS::LF_ROOMNAME)));
            break;
        case Service::ACTION::CREATE_USER:
             ZyncPrint("CREATE_USER:");
            return server_->AddUserToSQL(action->at(CONSTANTS::LF_NAME), action->at(CONSTANTS::LF_PASSWORD));
            break;
        case Service::ACTION::GET_USERS:            
            ZyncPrint("GET_USERS:");
            return server_->GetRoomUsersList(action->at(CONSTANTS::LF_ROOMNAME));
            break;
        case Service::ACTION::LOGIN:           
             ZyncPrint("LOGIN:");
            return server_->LoginUser(action ,  readbuf_, stream_ );
            break;
        case Service::ACTION::ROOM_LIST:           
           ZyncPrint("::ROOM_LIST:");
           return server_->GetRoomsList();
           break;
        }
        return ServiceChatroomServer::MakeAnswerError("UNRECOGNIZED ACTION SERVSESSION ", __func__ ,"");
    }
    catch (const std::exception &ex)
    {
        return ServiceChatroomServer::MakeAnswerError(ex.what(), __func__ , "");
    }
}

void ServerSession::StartExecuteAction(shared_task action)
{
    try
    {
        std::string responce_body;
        // Проверка есть ли действие и не пустое ли оно
        auto reason = ServiceChatroomServer::CHK_FieldExistsAndNotEmpty(*action, CONSTANTS::LF_ACTION);
        if (reason)
        {
            // Если нет - ошибка
            responce_body = ServiceChatroomServer::MakeAnswerError(*reason, "StartExecuteAction()Exc1", CONSTANTS::UNKNOWN);
            ZyncPrint("StartExecuteAction()Exc1");
        }
        else
        {
            // Если все ок - исполняем сессию
            responce_body = ExecuteReadySession(action);
        }
        if (responce_body.empty())
        {
            // При логине пользователя вернется "" значит закрываем сесссию
            return;
        }
        auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(responce_body)); 
            PublicWrite(std::move(responce));
    } // try
    catch (const std::exception &ex)
    {
        // ловим всевозможные исключения
        std::string responce_body = ServiceChatroomServer::MakeAnswerError(ex.what(), "StartExecuteAction()Exc2", CONSTANTS::UNKNOWN);
        ZyncPrint("StartExecuteAction()Exc2");
        auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(responce_body)); 
            PublicWrite(std::move(responce));
    }
};

void ServerSession::StartAfterReadHandle()
{
    /// ИЗВЛЕКАЕМ ЗНАЧЕНИЕ
    try
    {
        shared_task action = Service::ExtractSharedObjectsfromRequestOrResponce(request_);
        if (!action)
        {
            std::string responce_body = ServiceChatroomServer::MakeAnswerError("Action is nullptr", "StartAfterReadHandle1()", CONSTANTS::UNKNOWN);
            auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(responce_body)); 
            PublicWrite(std::move(responce));
            return;
        }
        StartExecuteAction(action);
    }
    // При исключении десереализации архива
    catch (const std::exception &ex)
    {
        std::string responce_body = ServiceChatroomServer::MakeAnswerError(ex.what(), "StartAfterReadHandle2()", CONSTANTS::UNKNOWN);
        auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(responce_body)); 
        ZyncPrint("StartAfterReadHandle2()");
        PublicWrite(std::move(responce));
    }
};