//
// Created by Dell on 2023/7/1.
//
#include <iostream>
#include <mutex>
#include <thread>

class Worker {
public:
    void Producer() {
        for (;;) {
            std::unique_lock<std::mutex> ul(mutex_);
            data_ = rand() % 100;
            std::cout << "produce data: " << data_ << std::endl;
            ready_ = true;
            ul.unlock();
            while (ready_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }

    void Consumer() {
        for (;;) {
            while (!ready_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            std::unique_lock<std::mutex> ul(mutex_);
            std::cout << "consume data: " << data_ << std::endl;
            ready_ = false;
            ul.unlock();
        }
    }

    void Join() {
        std::thread consumer_thread(&Worker::Consumer, this);
        std::thread produce_thread(&Worker::Producer, this);
        consumer_thread.join();
        produce_thread.join();
    }

private:
    std::mutex mutex_;
    int data_{0};
    bool ready_{false};
};

int main() {
    Worker worker;
    worker.Join();

    return 0;
}