#include "srv.h"



int main()
{
    
    try{
    setlocale(LC_ALL, "Ru-ru");
    const unsigned num_threads = std::thread::hardware_concurrency();
    net::io_context ioc(num_threads);
    MainServer server(ioc);
    server.init();
    server.Listen();
    Service::MtreadRunContext(ioc);
    }
    catch(const std::exception& ex){
        ZyncPrint("MAINSERVER EX", ex.what());
    }
}
