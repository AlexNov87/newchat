#pragma once
#include <string>
struct CONSTANTS
{
    // Actions
    static const std::string ACT_DISCONNECT;
    static const std::string ACT_GET_USERS;
    static const std::string ACT_SEND_MESSAGE;
    static const std::string ACT_LOGIN;
    static const std::string ACT_CREATE_ROOM;
    static const std::string ACT_ROOM_LIST;
    static const std::string ACT_CREATE_USER;
    static const std::string ACT_EARLIER_MESS;
    static const std::string ACT_USER_MESSAGE; 

    // Map Parameters
    static const std::string LF_ACTION;
    static const std::string LF_NAME;
    static const std::string LF_TOKEN;
    static const std::string LF_MESSAGE;
    static const std::string LF_REASON;
    static const std::string LF_RESULT;
    static const std::string LF_USERS;
    static const std::string LF_ROOMNAME;
    static const std::string LF_LOGIN;
    static const std::string LF_ROOMLIST;
    static const std::string LF_PASSWORD;
    static const std::string LF_LAST_MSG;

    static const std::string RF_SUCCESS;
    static const std::string RF_ERROR;

    //
    static const std::string LF_INITIATOR;
    static const std::string RF_ERR_INITIATOR_SERVER;
    static const std::string RF_ERR_INITIATOR_CHATROOM;
    static const std::string RF_ERR_PERMISSION_DENIDED;

    static const std::string LF_DIRECTION;
    static const std::string RF_DIRECTION_SERVER;
    static const std::string RF_DIRECTION_CHATROOM;

    static const std::string IP ; 
    static const std::string PORT; 
    static const std::string CHATROOMS;
    static const std::string UNKNOWN;
     

    ///@brief Длина токена
    static const size_t N_TOKEN_LEN;
    ///@brief
    static const size_t N_MAX_MESSAGE_LEN;

    static const char SERIAL_SYM;

private:
    CONSTANTS() {};
};