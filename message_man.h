#pragma once
#include<mutex>
#include<deque>
#include<string>
#include<iostream>
#include<sstream>

class MessageManager
{
struct AuhtorMessage{
    std::string author;
    std::string message;
};

public:
    void AddUserMessage(const std::string &message, std::string &author)
    {
        std::lock_guard<std::mutex> lg(mtx_msg_);
        std::cout << message << std::endl;
        ++messages_;
        std::stringstream strm;
        message_queue_.push_back({ author, message });
        ClearDeque();
    }

    template <typename T>
    void ServiceMessage(T message)
    {
        std::lock_guard<std::mutex> lg(mtx_service_);
        std::cout << message << std::endl;
    }

    std::string LastMessages()
    {
        std::lock_guard<std::mutex> lg(mtx_msg_);
        int loops = std::min(static_cast<int>(message_queue_.size()), 30);
        std::stringstream strm;
        strm << '{';
        int cnt;
        for(auto&& msg : message_queue_){
            ++cnt;
            strm << "{\"" << msg.author << " \" : \" " << msg.message << "\"}";
            if(cnt >= (loops - 1)){break;}
            strm << ',';
        }
        strm << '}';
        return strm.str();
    };

private:
    std::deque<AuhtorMessage> message_queue_;
    std::mutex mtx_msg_;
    std::mutex mtx_service_;
    size_t messages_;

private:
    void ClearDeque()
    {
        while (message_queue_.size() > 30)
        {
            message_queue_.pop_front();
        }
    }
};