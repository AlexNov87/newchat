#include "service.h"
using namespace std;

namespace Service
{
    const std::unordered_map<std::string, ACTION> Additional::action_scernario{
        {CONSTANTS::ACT_CREATE_ROOM, ACTION::CREATE_ROOM},
        {CONSTANTS::ACT_CREATE_USER, ACTION::CREATE_USER},
        {CONSTANTS::ACT_DISCONNECT, ACTION::DISCONNECT},
        {CONSTANTS::ACT_GET_USERS, ACTION::GET_USERS},
        {CONSTANTS::ACT_LOGIN, ACTION::LOGIN},
        {CONSTANTS::ACT_ROOM_LIST, ACTION::ROOM_LIST},
        {CONSTANTS::ACT_SEND_MESSAGE, ACTION::SEND_MESSAGE}};

    const std::unordered_set<std::string> Service::Additional::chatroom_actions{
        CONSTANTS::ACT_DISCONNECT, CONSTANTS::ACT_SEND_MESSAGE};

    const std::unordered_set<std::string> Service::Additional::server_actions{
        CONSTANTS::ACT_CREATE_ROOM, CONSTANTS::ACT_CREATE_USER,
        CONSTANTS::ACT_GET_USERS, CONSTANTS::ACT_LOGIN, CONSTANTS::ACT_ROOM_LIST};

    const std::unordered_set<std::string> Service::Additional::request_directions{
        CONSTANTS::RF_DIRECTION_SERVER, CONSTANTS::RF_DIRECTION_CHATROOM};

    std::mutex Service::Additional::mtx;

    void MtreadRunContext(net::io_context &ioc)
    {
        std::vector<jthread> run;
        for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            run.push_back(jthread([&ioc]
                                  { ioc.run(); }));
        }
        ioc.run();
    }

    std::string ReadFromFstream(std::ifstream &ifs)
    {
        if (!ifs)
        {
            std::cerr << "Bad Filestream\n";
            return CONSTANTS::RF_ERROR;
        }
        std::stringstream strm;

        while (ifs)
        {
            std::string tmp;
            std::getline(ifs, tmp);
            strm << tmp;
        }
        return strm.str();
    };
}

namespace Service
{
    bool IsAliveSocket(tcp::socket &sock)
    {
        return sock.is_open();
    }

    bool IsAliveSocket(shared_socket sock)
    {
        return IsAliveSocket(*sock);
    };

    void ShutDownSocket(tcp::socket &sock)
    {
        boost::system::error_code ec;
        sock.cancel(ec);
        if (ec)
        {
            std::cout << ec.what() << std::endl;
        }
        sock.shutdown(sock.shutdown_both, ec);
        if (ec)
        {
            std::cout << ec.what() << std::endl;
        }
        sock.close();
        if (ec)
        {
            std::cout << ec.what() << std::endl;
        }
    };
    void ShutDownSocket(shared_socket sock)
    {
        ShutDownSocket(*sock);
    };
}

namespace Service
{
    shared_strand MakeSharedStrand(net::io_context &ioc)
    {
        return std::make_shared<strand>(net::make_strand(ioc));
    }
    std::shared_ptr<MutableBufferHolder> MakeSharedMutableBuffer()
    {
        return std::shared_ptr<Service::MutableBufferHolder>();
    };
    std::shared_ptr<net::streambuf> MakeSharedStreambuf()
    {
        return std::make_shared<net::streambuf>();
    };
   
    std::shared_ptr<beast::flat_buffer> MakeSharedFlatBuffer()
    {
        return std::make_shared<beast::flat_buffer>();
    };

    request MakeRequest(http::verb verb, int version, std::string body)
    {
        request req{verb, "/s/d/d/", version};
        req.set(boost::beast::http::field::host, "127.0.0.1");
        req.set(boost::beast::http::field::content_type, "text/html");
        req.body() = std::move(body);
        req.keep_alive(true);
        req.prepare_payload();
        return req;
    };

    response MakeResponce(int version, bool keep_alive, beast::http::status status, std::string body)
    {
        response resp{status, version};
        resp.keep_alive(keep_alive);
        resp.set(boost::beast::http::field::content_type, "text/html");
        resp.body() = std::move(body);
        resp.prepare_payload();
        return resp;
    }

}