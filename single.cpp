//
// Created by Dell on 2023/7/1.
//
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class Worker {
public:
    void Consumer() {
        for (;;) {
            std::unique_lock<std::mutex> ul(mutex_);
            cv_.wait(ul, [this](){
                return ready_;
            });
            std::cout << "consume data: " << data_ << std::endl;
            ready_ = false;
            ul.unlock();
            cv_.notify_one();
        }
    }

    void Producer() {
        for (;;) {
            std::unique_lock<std::mutex> ul(mutex_);
            data_ = rand() % 100;
            std::cout << "produce data: " << data_ << std::endl;
            ready_ = true;
            ul.unlock();
            cv_.notify_one();
            ul.lock();
            cv_.wait(ul, [this](){
                return !ready_;
            });
        }
    }

    void Join() {
        std::thread consumer_thread(&Worker::Consumer, this);
        std::thread producer_thread(&Worker::Producer, this);
        consumer_thread.join();
        producer_thread.join();
    }
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool ready_{false};
    int data_{0};
};

//int main() {
//    Worker w;
//    w.Join();
//
//    return 0;
//}
