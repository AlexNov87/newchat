#pragma once
#include "tokenizer.h"
#include "message_man.h"
#include "service.h"

class MainServer;
class Chatroom;
struct Chatuser;

class AbstractSession : public std::enable_shared_from_this<AbstractSession>
{
    void Read();
    void OnRead(err ec, size_t bytes);
    void OnWrite(bool keep_alive, err ec, size_t bytes);
    void Close();

    virtual std::string WhoAmI() { return "I AM ABSTRACT........"; };
    virtual std::string Iread() { return "ABSTRACT I READ"; };
    virtual std::string Iwrite() { return "ABSTRACT I WRITE"; };

    static std::atomic_int exempslars;

    std::mutex mtx_use_buf_;
    std::mutex mtx_use_write_;

    friend class Chatroom;
    friend class Chatuser;

protected:
    shared_flatbuf readbuf_ = Service::MakeSharedFlatBuffer();
    shared_stream stream_ = nullptr;
    request request_;
    AbstractSession(shared_stream stream)
        : stream_(stream) { ZyncPrint("PROTOSESS: " + std::to_string(++exempslars) + " IS CONSTRUCTING"); };
    virtual void StartAfterReadHandle() {};
    virtual std::string ExecuteReadySession(shared_task action) { return ""; };
    void Write(std::string respbody, http::status status = http::status::ok);

public:
    void Run();
    virtual ~AbstractSession() { ZyncPrint("SESSION CLOSED....."); };
    shared_stream GetStream() { return stream_; }
};

class ServerSession : public AbstractSession
{
    MainServer *server_;
    static std::atomic_int exempslars_s;
    friend class Chatroom;
    friend class Chatuser;
    void StartAfterReadHandle() override;
    void StartExtractAction();
    void StartExecuteAction(shared_task action);
    std::string WhoAmI() override { return "I AM SERVERSESS........"; };

public:
    std::string ExecuteReadySession(shared_task action) override;
    ServerSession(MainServer *server, shared_stream stream)
        : AbstractSession(stream), server_(server) {};
};

class ChatRoomSession : public AbstractSession
{
    Chatroom *chatroom_ = nullptr;
    friend class Chatuser;

public:
    std::string ExecuteReadySession(shared_task action) override;
    ChatRoomSession(Chatroom *chat, shared_stream stream)
        : AbstractSession(stream), chatroom_(chat) {};
};

struct Chatuser : public AbstractSession
{
    // stream, self, std::move(name), mainserv_->ioc_
    Chatuser(shared_stream stream, std::weak_ptr<Chatroom> room, std::string name,
             net::io_context &io) : AbstractSession(stream),
                                    room_(room), name_(std::move(name)), strand_(Service::MakeSharedStrand(io)) {}

    std::weak_ptr<Chatroom> room_;
    std::string name_;
    shared_strand strand_;

    void StartAfterReadHandle() override;
    void BindAnotherReadBuffer(shared_flatbuf buffer);

    std::string ExecuteReadySesion(shared_task action);
    std::string WhoAmI() override { return "I AM CHATUSER........"; };
};

class Chatroom : public std::enable_shared_from_this<Chatroom>
{

    friend class MainServer;
    friend class ChatRoomSession;
    friend struct Chatuser;

    MainServer *mainserv_;
    std::string name_;

    std::unordered_map<std::string, std::shared_ptr<Chatuser>> users_;
    std::mutex mtx_;
    MessageManager msg_man_;

public:
    Chatroom(MainServer *serv, const std::string &name) : mainserv_(serv), name_(name) {}

private:
    bool HasToken(const std::string &token);
    bool AddUser(shared_stream stream, shared_flatbuf buffer, std::string name, std::string token);
    void SendMessages(const std::string &token, const std::string &name, const std::string &message);
    void DeleteUser(std::string token);
    std::string RoomMembers();
};

class MainServer
{
    friend class Chatroom;
    friend class ServerSession;
    friend class AbstactSession;

    net::io_context &ioc_;
    tcp::acceptor acceptor_;
    tcp::endpoint endpoint_;
    Service::TokenGen tokezier_;
    std::unordered_map<std::string, std::shared_ptr<Chatroom>> rooms_;
    std::unordered_map<std::string, std::string> users_hash_;

    std::mutex mtx_lock_user_operations_;
    std::mutex mtx_lock_sql_operations_;

public:
    MainServer(net::io_context &ioc);
    void Listen();
    void init();
    void PrintRooms();

private:
    // В работе....
    bool AlreadyUserRegistered(const std::string &name);                          // УБРАТЬ ЗАГЛУШКУ
    bool IsAutorizatedUser(const std::string &name, const std::string &passhash); // УБРАТЬ ЗАГЛУШКУ
    // Действия
    std::string GetRoomUsersList(const std::string &roomname);
    std::string GetRoomsList();
    std::string CreateRoom(std::string room);
    std::string LoginUser(shared_task action, shared_flatbuf buffer, shared_stream stream);
    std::string AddUserToSQL(const std::string &name, const std::string &passhash);
};


/*
auto data = readbuf_->data();
        std::string result(boost::asio::buffer_cast<const char *>(data), boost::asio::buffer_size(data));
        ZyncPrint(WhoAmI(), "(READ) BUFFER----->", result, "\n");
*/