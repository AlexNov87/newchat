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
std::atomic_int cnt;
auto values = GetStringValues();

void Read();

void test1()
{

  try
  {
    auto buf = Service::MakeSharedStreambuf();

    for (auto &&str : *values)
    {

      net::async_write(*socket__, net::buffer(str), [](err ec, size_t bytes)
                       { Read(); });
    }
  }
  catch (const std::exception &ex)
  {
    std::cout << ex.what();
    system("pause");
  }
}

void test2()
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
  for (int q = 0; q < 1; ++q)
  {
    for (int i = 0; i < o.size(); ++i)
    {
      ZyncPrint("ITER " + std::to_string(i));
      tcp::socket sock{ioc};
      InitSocket(sock);

      auto req = Service::MakeRequest(http::verb::get, 11, o[i]);
      http::write(sock, req);
      beast::flat_buffer buf;
      response resp;
      http::read(sock, buf, resp);
      auto data = Service::ExtractSharedObjectsfromRequestOrResponce(resp);
      Service::PrintUmap(*data);
      sock.close();
    }
  }
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

std::mutex m;
void Do(shared_socket sock, std::string act)
{

  auto lam = [act, sock]
  {
    auto req = Service::MakeRequest(http::verb::get, 11, UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "YANDEX"));

    http::async_write(*sock, req, [sock](err ec, size_t bytes)
                      {
                        beast::flat_buffer buf;
                        auto sh = std::make_shared<response>();
                        response rs;
                        http::async_read(*sock, buf, *sh, [sh](err ec, size_t bytes)
                                         {
      if(!ec){
      auto data = Service::ExtractSharedObjectsfromRequestOrResponce(*sh);
      std::lock_guard<std::mutex>lg(m);
      Service::PrintUmap(*data);
      
      } else {
        ZyncPrint("ERR", ec.message());}; }); });
  };
  net::post(*strand__, lam);
};

void Do2(shared_socket sock, std::string act)
{
  err ec;
  auto req = Service::MakeRequest(http::verb::get, 11, UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "YANDEX"));
  ZyncPrint("WRITE.............");
  http::write(*sock, req, ec);
};


void Read()
{
  auto sb = Service::MakeSharedFlatBuffer();
  std::shared_ptr<response> req = std::make_shared<response>();
  auto handler = [sb, req](err ec, size_t bytes)
  {
    ZyncPrint("LAMPDA READ...............");
    if (ec)
    {
      ZyncPrint(ec.what());
      system("pause");
      return;
    }

    // ZyncPrint("->" + Service::ExtractStrFromStreambuf(*sb, bytes) + "<-");
    auto i = Service::ExtractSharedObjectsfromRequestOrResponce(*req);
    Service::PrintUmap(*i);
    sb->consume(bytes);
    Read();
  };
  http::async_read(*socket__, *sb, *req, handler);
}

void test3()
{

  Do2(socket__, UserInterface::US_SrvMakeObjLogin("RRAT", "jijjiw", "YANDEX"));
  Do2(socket__, UserInterface::US_SrvMakeObjGetUsers("YANDEX"));
  
};

int main()
{
  InitSocket(*socket__);
  Read();
 //  Service::MtreadRunContext(ioc);  
  
  net::post(ioc,[]{ test3();});
  ioc.run();

  // запуск ioc
 
}