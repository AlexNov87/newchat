#include "srv.h"

bool Chatroom::HasToken(const std::string &token)
{
    return users_.contains(token);
}

bool Chatroom::AddUser(shared_stream stream , shared_flatbuf buffer, std::string name,
                       std::string token)
{
    std::lock_guard<std::mutex> lg(mtx_);
   
    bool success = false;
    auto self = weak_from_this();
    
    /*Чатюзер - это по сути непрерывная сессия по которой могут поступать сообщения как
     от самого сокет-холдера на другой стороне, так и от других сессий... Обращение на
     создание юзера это создание такой сессии, из стрима старой сессии мы извлекаем сокет,
     создаем новый стрим.
     std::make_shared<Chatuser>(std::make_shared<beast::tcp_stream>(std::move(stream->socket()))
    */
    auto it = users_.insert({token, std::make_shared<Chatuser>(stream, self, std::move(name))});
    success = it.second;

    if (success)
    {
        users_.at(token)->BindAnotherReadBuffer(buffer);
        
        //Не запускаем прослушивание стрима , он был запущен в Сервер - сессии 
       // users_.at(token)->Run();
         
        auto str = ServiceChatroomServer::Srv_MakeSuccessLogin (token, this->name_, this->msg_man_.LastMessages());
        auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(str));
        users_.at(token)->PublicWrite(std::move(responce));


         for(int i=0; i < 10; ++ i){
            auto s = ServiceChatroomServer::MakeAnswerError("TST", "TST", "TST");
            auto rsp = Service::MakeResponce(11, true, http::status::ok, std::move(s));
            users_.at(token)->PublicWrite(std::move(rsp));
         }

        // ЛОГИРУЕМ СИТЕМНОЕ СООБЩЕНИЕ
        msg_man_.ServiceMessage(users_.at(token)->name_ + " IS CONNECTED");
        return success;
    }
    return success;
}

void Chatroom::SendMessages(const std::string &token, const std::string &name, const std::string &message)
{
     
    // Создаем тело ответа
    auto str = ServiceChatroomServer::Chr_MakeSuccessUserMessage(name, message);
    auto responce = Service::MakeResponce(11, true, http::status::ok, std::move(str));
    
    std::lock_guard<std::mutex> lg(mtx_);
    std::cout << users_.at(token)->name_ << " : " << message << '\n';
    // Рассылка
    for (auto &&[token, chatuser] : users_)
    {
        // Если сокет недоступен
        if (!Service::IsAliveSocket(chatuser->GetStream()->socket()))
        {
            continue;
        }
        chatuser->PublicWrite(responce);
    };
}

void Chatroom::DeleteUser(std::string token)
{
    std::lock_guard<std::mutex> lg(mtx_);
    users_.at(token)->Close();
    users_.erase(token);
}

std::string Chatroom::RoomMembers()
{
    std::ostringstream oss;
    oss << "[ ";
    size_t nowpos = 0;

    std::unique_lock<std::mutex> ul(mtx_);
    {
        for (auto &&[token, chatuser] : users_)
        {
            oss << '"' << chatuser->name_ << '"';
            ++nowpos;
            if (nowpos >= users_.size() - 1)
            {
                break;
            }
            oss << " , ";
        }
    }
    ul.unlock();
    oss << " ]";
    return oss.str();
}
