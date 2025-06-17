#include "srv.h"
std::atomic_int AbstractSession::exempslars = 0;
////////////////////////////////

void AbstractSession::Read()
{
    request_ = {};
    //ПРОВЕРЯЕМ ЖИВ ЛИ СТРИМ
    if (!stream_)
    {
        ZyncPrint("STREAM SESSION IS DAMAGED........READ()");
        return;
    }
    //  ПРОВЕРЯЕМ ЖИВ ЛИ СОКЕТ
    if (!Service::IsAliveSocket(stream_->socket()))
    {
        ZyncPrint("SOCKET IS DAMAGED...........READ()");
        Service::ShutDownSocket(stream_->socket());
        return;
    }

    // Начинаем асинхронноен чтение
    //  std::lock_guard<std::mutex> lg(mtx_use_buf_);
    http::async_read(*stream_, *readbuf_, request_,
                     beast::bind_front_handler(&AbstractSession::OnRead, shared_from_this())); // async read until
};

void AbstractSession::OnRead(err ec, size_t bytes)
{
    if (!ec)
    { // Обрабатываем прочитанные данные
        StartAfterReadHandle();
    }
    else
    {
        ZyncPrint("ON READ----> " + ec.message());
    }
};

void AbstractSession::Run()
{
    if (!stream_)
    {
        ZyncPrint("THE STREAM IS DAMAGED...........RUN()");
        return;
    }
    //Асинхронно ставим на прослушку
    net::dispatch(strand_,
                  beast::bind_front_handler(
                      &AbstractSession::Read,
                      shared_from_this()));
};

void AbstractSession::PublicWrite(std::string message)
{
    //Ставим сообщение со стороны в очередь безопасно на запись
    net::post(strand_, [self = shared_from_this(), msg = std::move(message)]()
              { 
                self->Write(std::move(msg)); 
              });
}


void AbstractSession::Write(std::string responce_body, http::status status)
{
    try
    {
        //Ставим в очередь сообщение на запись
        write_queue_.push_back(std::move(responce_body));

        // Если уже идет запись, просто добавляем в очередь и выходим.
        // OnWrite запустит следующую запись.
        if (is_writing_) { return;}
        
        // Запускаем цикл записи
        DoWrite();
    }
    catch (const std::exception &ex)
    {
        ZyncPrint("WriteToSocketEXCERPTION", ex.what());
    }
};

void AbstractSession::DoWrite()
{
    // Мы уже внутри strand'а
    if (write_queue_.empty())
    {
        is_writing_ = false;
        Read();  //В нейронк
        return;
    }
    response rsp(Service::MakeResponce(
        11, true, http::status::ok, std::move(write_queue_.front())));

    is_writing_ = true;
    
    ZyncPrint("Now WIOLL WRITE", rsp.body());
    
    // Берем первое сообщение из очереди и отправляем его
    http::async_write(*stream_, rsp,
                      beast::bind_front_handler(&AbstractSession::OnWrite, shared_from_this(), true));
}

void AbstractSession::OnWrite(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred)
{

    if (!keep_alive)
    {
        Close();
    }

    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        ZyncPrint("ERROR ON WRITE.................", ec.message());
        return;
    }
    // Убираем из очереди то, что только что отправили
   
    write_queue_.pop_front();
    // Запускаем запись следующего сообщения из очереди, если оно есть
    DoWrite();
}

void AbstractSession::Close()
{
    beast::error_code ec;
    stream_->socket().shutdown(tcp::socket::shutdown_send, ec);
}
