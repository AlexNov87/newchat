#include "srv.h"

    std::string Chatuser::ExecuteReadySesion(shared_task action)
    {

        try{
        auto reason = ServiceChatroomServer::CHK_FieldExistsAndNotEmpty(*action, CONSTANTS::LF_ACTION);
        if (reason)
        {
            return ServiceChatroomServer::MakeAnswerError(*reason, __func__, CONSTANTS::UNKNOWN);
        }
        std::string act_to_send = action->at(CONSTANTS::LF_ACTION);

        if (action->at(CONSTANTS::LF_DIRECTION) == CONSTANTS::RF_DIRECTION_CHATROOM)
        {
            auto reason = ServiceChatroomServer::CHK_Chr_CheckErrorsChatRoom(*action);
            if (reason)
            {
                return ServiceChatroomServer::MakeAnswerError(*reason, __func__, act_to_send);
            }
            // socket использоваться не будет, передан только в качестве реализации интерфейса
           ChatRoomSession session(room_.lock().get(), stream_);
           return session.ExecuteReadySession(action);
        }
        else if (action->at(CONSTANTS::LF_DIRECTION) == CONSTANTS::RF_DIRECTION_SERVER)
        {

            auto reason = ServiceChatroomServer::CHK_Chr_CheckErrorsChatServer(*action);
            if (reason)
            {
                return ServiceChatroomServer::MakeAnswerError(*reason, __func__, act_to_send);
            }
            if(action->at(CONSTANTS::LF_ACTION) == CONSTANTS::ACT_CREATE_USER){
                return ServiceChatroomServer::MakeAnswerError("YOU CAN'T CREATE USERS WLILE YOU CONNECTED" , __func__, act_to_send);
            }

            if(action->at(CONSTANTS::LF_ACTION) == CONSTANTS::ACT_LOGIN){
                return ServiceChatroomServer::MakeAnswerError("YOU CAN'T LOGIN WLILE YOU CONNECTED" , __func__, act_to_send);
            }
            
           // socket использоваться не будет, передан только в качестве реализации интерфейса
           ServerSession session(room_.lock().get()->mainserv_, stream_);
           return session.ExecuteReadySession(action);
        }
        }
        catch(const std::exception &ex){
            return ServiceChatroomServer::MakeAnswerError("Exception Chatuser::ReadySesion", __func__, CONSTANTS::UNKNOWN); 
        }

        return ServiceChatroomServer::MakeAnswerError("", __func__, ""); 
    };


    void Chatuser::StartAfterReadHandle()
    {
        auto action = Service::ExtractSharedObjectsfromRequestOrResponce(request_);
        auto resp_body = ExecuteReadySesion(action);
        auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(resp_body)); 
        PublicWrite(std::move(responce));
    };

    void Chatuser::BindAnotherReadBuffer(shared_flatbuf buffer)
    {
        readbuf_ = buffer;
    }


