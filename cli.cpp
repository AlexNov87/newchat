#include "service.h"
#include "tokenizer.h"
#include <boost/asio.hpp>
#include <iostream>

namespace net = boost::asio;
using tcp = net::ip::tcp;
int port = 1601;

std::shared_ptr<std::ofstream> ofs = std::make_shared<std::ofstream>("log!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.txt");

net::io_context ioc(16);
shared_strand strand__ = Service::MakeSharedStrand(ioc);
shared_strand strandwr__ = Service::MakeSharedStrand(ioc);

shared_socket socket__ = Service::MakeSharedSocket(ioc);
boost::system::error_code ec;
auto endpoint = tcp::endpoint(net::ip::make_address("127.0.0.1"), 1601);
std::mutex mtx;
void InitSocket(tcp::socket &sock);

std::shared_ptr<std::vector<std::string>> GetStringValues()
{
  Service::TokenGen gen;
  std::vector<std::string> objs{
      UserInterface::US_ChrMakeSendMessage(gen.GenerateHEXToken(), "22sssssssswdqeefvqwef222222222222222222222222222222"),
      UserInterface::US_ChrMakeObjDisconnect(gen.GenerateHEXToken()),
      UserInterface::US_SrvMakeObjCreateRoom("YANDEX"),
      UserInterface::US_SrvMakeObjCreateUser("RRAT", "hjsjklk;l"),
      UserInterface::US_SrvMakeObjGetUsers("RRAT"),
      UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "kjjolpdpw"),
      UserInterface::US_SrvMakeObjRoomList()

  };
  return std::make_shared<std::vector<std::string>>(std::move(objs));
}
void Read();

void _______()
{
  std::vector<std::string> o{

      // UserInterface::US_ChrMakeSendMessage(gen.GenerateHEXToken(), "22sssssssswdqeefvqwef222222222222222222222222222222"),
      // UserInterface::US_ChrMakeObjDisconnect(gen.GenerateHEXToken()),
      UserInterface::US_SrvMakeObjCreateRoom("YANDEX!"),
      UserInterface::US_SrvMakeObjCreateUser("RRAT", "hjsjklk;l"),
      UserInterface::US_SrvMakeObjGetUsers("YANDEX"),
      UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "YANDEX"),
      UserInterface::US_SrvMakeObjRoomList()

  };
}

void InitSocket(tcp::socket &sock)
{
  sock.open(endpoint.protocol(), ec);
  if (ec)
  {
    std::cerr << "Open error: " << ec.message() << std::endl;
  }

  sock.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec)
  {
    std::cerr << "Set option error: " << ec.message() << std::endl;
  }

  sock.connect(endpoint, ec);
  if (ec)
  {
    std::cerr << "COnnect error: " << ec.message() << std::endl;
  }
}

void Do3(shared_socket sock, std::string act)
{
  err ec;
  auto req = Service::MakeRequest(http::verb::get, 11, act);

  net::post(*strandwr__, [sock, req]
            { http::async_write(*sock, req, [](err ec, size_t bytes)
                                { ZyncPrint("WRITE COMPLETE............."); }); });
  // http::write(*sock, req, ec);
};

void Read()
{
  auto sb = Service::MakeSharedFlatBuffer();
  std::shared_ptr<response> req = std::make_shared<response>();
  auto handler = [sb, req](err ec, size_t bytes)
  {
    ZyncPrint("READ COMPLETE...............");
    if (ec)
    {
      ZyncPrint("LAMPDA MIST...............");
      ZyncPrint(ec.what(), "BODY", req->body());
      system("pause");
    }
    // ZyncPrint("->" + Service::ExtractStrFromStreambuf(*sb, bytes) + "<-");
    auto i = Service::ExtractSharedObjectsfromRequestOrResponce(*req);
    Service::PrintUmap(*i);
    Read();
  };

  net::post(*strandwr__, [req, sb, handler]
            { http::async_read(*socket__, *sb, *req, handler);});
}

void test3()
{

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "YANDEX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjCreateRoom("YAARRRR")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjCreateRoom("YANDEX")); });
  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjLogin("OOORRAT", "jijjiw", "YANDEX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjLogin("UUUUUUUURRAT", "jijjiw", "YANDEX")); });
  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjRoomList()); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDEX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDOOOOOX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDOOOIIIIIIIIIIIIIIIIIIIIIIX")); });
};

void test5()
{

  //   net::post(*strandwr__ ,[]{
  //        Do3(socket__, UserInterface::US_SrvMakeObjRoomList());
  //  });

  //  net::post(*strandwr__ ,[]{
  //       Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDEX"));
  //  });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDOOOOOX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDOOOOOX")); });

  net::post(*strandwr__, []
            { Do3(socket__, UserInterface::US_SrvMakeObjCreateUser("UUUUUUUUUUUU", "jshi2jojkojkp")); });
}

int main()
{
  InitSocket(*socket__);

  net::post(*strandwr__, []
            { Read(); });

  net::post(*strandwr__, []
            { 
      test5();
      test3(); });

  Service::MtreadRunContext(ioc);
}