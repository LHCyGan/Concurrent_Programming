//
// Created by Dell on 2023/7/6.
//
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

class Worker {
public:
    Worker(int max_queue_size) : max_queue_size_(max_queue_size) {}
    void Consumer() {
        for (;;) {
            std::unique_lock<std::mutex> ul(consumer_mutex_);
            cv_consumer_.wait(ul, [this]()  {
                return!queue_.empty();
            });
            int data = queue_.front();
            queue_.pop();
            std::cout << "Consumer" << std::this_thread::get_id() << "data: " << data << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            ul.unlock();
            cv_producer_.notify_one();
        }
    }

    void Producer() {
        for (;;) {
            std::unique_lock<std::mutex> ul(producer_mutex_);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            int data = rand() % 100;
            queue_.push(data);
            std::cout << "Producer: " << std::this_thread::get_id() << " data: " << data << std::endl;
            ul.lock();
            cv_consumer_.notify_all();
            ul.lock();
            cv_producer_.wait(ul, [this]()  {
                return queue_.size() < max_queue_size_;
            });
        }
    }

    void Join(int num_consumers) {
        std::vector<std::thread> consumer_thread;
        std::thread producer_thread(&Worker::Producer, this);
        for (int i = 0; i < num_consumers; i ++) {
            consumer_thread.emplace_back(&Worker::Consumer, this);
        }

        for (auto& thread : consumer_thread) {
            thread.join();
        }
        producer_thread.join();
    }
private:
    std::mutex producer_mutex_;
    std::mutex consumer_mutex_;
    std::condition_variable cv_producer_;
    std::condition_variable cv_consumer_;

    std::queue<int> queue_;
    int max_queue_size_;
};

int main() {
    Worker w(10);
    w.Join(2);
    return 0;
}

