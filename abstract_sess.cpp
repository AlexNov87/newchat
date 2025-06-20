// pvs pvs
// pvs pvs

#include "srv.h"
std::atomic_int AbstractSession::exempslars = 0;
////////////////////////////////

void AbstractSession::Read()
{
    std::lock_guard<std::mutex> lg(mtx_read_);
    if (is_reading_)
    {
        return;
    };

    request_ = {};
    // ПРОВЕРЯЕМ ЖИВ ЛИ СТРИМ
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
    is_reading_.exchange(true);
    // Начинаем асинхронноен чтение
    //  std::lock_guard<std::mutex> lg(mtx_use_buf_);
    http::async_read(*stream_, *readbuf_, request_,
                     beast::bind_front_handler(&AbstractSession::OnRead, shared_from_this())); // async read until
};

void AbstractSession::OnRead(err ec, size_t bytes)
{
    is_reading_.exchange(false);
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
    // Асинхронно ставим на прослушку
    net::dispatch(strand_,
                  beast::bind_front_handler(
                      &AbstractSession::Read,
                      shared_from_this()));
};

void AbstractSession::PublicWrite(response responce)
{
    // Ставим сообщение со стороны в очередь безопасно на запись
    net::post(strand_, [self = shared_from_this(), msg = std::move(responce)]()
              { self->Write(std::move(msg)); });
}

void AbstractSession::Write(response responce)
{
    try
    {

        std::lock_guard<std::mutex> lock(mtx_);
        write_queue_.push_back(std::move(responce));

        if (!is_writing_) // Атомарно устанавливаем is_writing_ в true и проверяем предыдущее значение.
        {
            is_writing_.exchange(true);
            net::post(strand_, beast::bind_front_handler(&AbstractSession::DoWrite, shared_from_this()));
        }
    }
    catch (const std::exception &ex)
    {
        ZyncPrint("WriteEXCERPTION", ex.what());
    }
};

void AbstractSession::DoWrite()
{

    try
    {

        std::lock_guard<std::mutex> lock(mtx_);
        if (write_queue_.empty())
        {
            is_writing_.store(false); //  Обязательно сбрасываем флаг, когда очередь пуста
            net::post(strand_, [self = shared_from_this()]
                      { self->Read(); }); // В нейронк
            return;
        }

        // ZyncPrint("NOW WILL WRITE",  write_queue_.front());
        response rsp = std::move(write_queue_.front()); // Copy the response
        write_queue_.pop_front();                       // Удаляем из очереди после копирования

        // Не знаю где гонка, но проблему решает
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        http::async_write(*stream_, std::move(rsp), // move the copy
                          beast::bind_front_handler(&AbstractSession::OnWrite, shared_from_this(), true));
    }
    catch (const std::exception &ex)
    {
        ZyncPrint("DOWriteEXCERPTION", ex.what());
    }
}

void AbstractSession::OnWrite(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred)
{

    try
    {
        if (!keep_alive)
        {
            Close();
            return;
        }

        boost::ignore_unused(bytes_transferred);

        if (ec)
        {
            ZyncPrint("ERROR ON WRITE.................", ec.message());
            return;
        }

        // net::post - Запускаем DoWrite в strand, *после* завершения текущей операции.
        net::post(strand_, beast::bind_front_handler(&AbstractSession::DoWrite, shared_from_this()));
    }
    catch (const std::exception &ex)
    {
        ZyncPrint("OnWriteEXCERPTION", ex.what());
    }
}

void AbstractSession::Close()
{
    beast::error_code ec;
    stream_->socket().shutdown(tcp::socket::shutdown_send, ec);
    if (ec)
    {
        ZyncPrint("CLOSE1", ec.message());
    }
    stream_->socket().close(ec);
    if (ec)
    {
        ZyncPrint("CLOSE2", ec.message());
    }
}
