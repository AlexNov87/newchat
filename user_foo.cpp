#include "service.h"
using namespace std;

    namespace UserInterface
    {
        void ChrSetChatroomDirection(unordered_map<string, string> &action)
        {
            action[CONSTANTS::LF_DIRECTION] = CONSTANTS::RF_DIRECTION_CHATROOM;
        }
        std::string US_ChrMakeObjDisconnect(std::string token) //sz3
        {
            unordered_map<string, string> res{
                {CONSTANTS::LF_ACTION, CONSTANTS::ACT_DISCONNECT},
                {CONSTANTS::LF_TOKEN, token}
            };
            ChrSetChatroomDirection(res);
            return Service::SerializeUmap(res);
        };

        std::string US_ChrMakeSendMessage(std::string token, std::string message) //sz4
        {
            unordered_map<string, string> res{
                {CONSTANTS::LF_ACTION, CONSTANTS::ACT_SEND_MESSAGE},
                {CONSTANTS::LF_TOKEN, token},
                {CONSTANTS::LF_MESSAGE, message}};
            ChrSetChatroomDirection(res);
            return Service::SerializeUmap(res);
        };

    }

    namespace UserInterface
    {
        void ChrSetServerDirection(unordered_map<string, string> &action)
        {
            action[CONSTANTS::LF_DIRECTION] = CONSTANTS::RF_DIRECTION_SERVER;
        }

        std::string US_SrvMakeObjGetUsers(std::string name) // sz3
        {
            unordered_map<string, string> res{
                {CONSTANTS::LF_ACTION, CONSTANTS::ACT_GET_USERS},
                {CONSTANTS::LF_ROOMNAME, std::move(name)}};
            ChrSetServerDirection(res);
            return Service::SerializeUmap(res);
        };

        std::string US_SrvMakeObjLogin(std::string name, std::string password, std::string roomname) //sz6
        {
            unordered_map<string, string> res{
             {CONSTANTS::LF_ACTION, CONSTANTS::ACT_LOGIN},
             {CONSTANTS::LF_NAME, std::move(name)},
             {CONSTANTS::LF_ROOMNAME , std::move(roomname)},
             {CONSTANTS::LF_PASSWORD , std::move(password)} 
            };
            ChrSetServerDirection(res);
            return Service::SerializeUmap(res);
        };
        ///@brief Сериализованный объект для создания пользователя
        std::string US_SrvMakeObjCreateUser(std::string name, std::string password) //sz4
        {
            unordered_map<string, string> res{
                {CONSTANTS::LF_ACTION, CONSTANTS::ACT_CREATE_USER},
                {CONSTANTS::LF_NAME, std::move(name)},
                {CONSTANTS::LF_PASSWORD, std::move(password)}
            };
            ChrSetServerDirection(res);
            return Service::SerializeUmap(res);
        };
        ///@brief Сериализованный объект для получения
        std::string US_SrvMakeObjCreateRoom(std::string name) //sz3 
        {
            unordered_map<string, string> res {
               {CONSTANTS::LF_ACTION, CONSTANTS::ACT_CREATE_ROOM},
               {CONSTANTS::LF_ROOMNAME, std::move(name)}
            };
            
            ChrSetServerDirection(res);
            return Service::SerializeUmap(res);
        };
        ///@brief Сериализованный объект для получения
        std::string US_SrvMakeObjRoomList() //sz2
        {
            unordered_map<string, string> res{
                {CONSTANTS::LF_ACTION, CONSTANTS::ACT_ROOM_LIST}
            };
            ChrSetServerDirection(res);
            return Service::SerializeUmap(res);
        };

    }

