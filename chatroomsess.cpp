#include "srv.h"

std::string ChatRoomSession::ExecuteReadySession(shared_task action)
{

    Service::ACTION current_action =
        Service::Additional::action_scernario.at(CONSTANTS::LF_ACTION);

    switch (current_action)
    {
    case Service::ACTION::SEND_MESSAGE:
    {
        if (!chatroom_->HasToken(action->at(CONSTANTS::LF_TOKEN)))
        {
            return ServiceChatroomServer::MakeAnswerError("TOKEN NOT FOUND", __func__, CONSTANTS::ACT_SEND_MESSAGE);
        }
        //Добавил имя чтобы попозже поставиль lock-guard
        chatroom_->SendMessages(action->at(CONSTANTS::LF_TOKEN),action->at(CONSTANTS::LF_NAME), action->at(CONSTANTS::LF_MESSAGE));
        return ServiceChatroomServer::Chr_MakeSuccessSendMessage();
    }
    break;
    case Service::ACTION::DISCONNECT:
    {
        if (!chatroom_->HasToken(action->at(CONSTANTS::LF_TOKEN)))
        {
            return ServiceChatroomServer::MakeAnswerError("TOKEN NOT FOUND", __func__, CONSTANTS::ACT_DISCONNECT);
        }
        chatroom_->DeleteUser(action->at(CONSTANTS::LF_TOKEN));
        return ServiceChatroomServer::MakeAnswerError("USER DELETED", __func__, CONSTANTS::ACT_DISCONNECT);
    }
    break;
    }
    return ServiceChatroomServer::MakeAnswerError("UNKNOWN ACTION", __func__ , "");
}