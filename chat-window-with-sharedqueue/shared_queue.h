// shared_queue.h
#ifndef SHAREDQUEUE_H
#define SHAREDQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <thread>
#include <iostream>

template <typename T>
class SharedQueue {
public:
    using Callback = std::function<void(const T&)>;

    SharedQueue() = default;
    ~SharedQueue();

    void subscribe(Callback callback);
    void publish(const T& message);

private:
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::queue<T> message_queue_;
    Callback subscriber_callback_;
    bool stop_flag_ = false;

    void processMessages();
};

template <typename T>
SharedQueue<T>::~SharedQueue() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_flag_ = true;
    }
    queue_cv_.notify_all();
}

template <typename T>
void SharedQueue<T>::subscribe(Callback callback) {
    subscriber_callback_ = callback;
    std::thread(&SharedQueue::processMessages, this).detach();
}

template <typename T>
void SharedQueue<T>::publish(const T& message) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
    }
    queue_cv_.notify_one();
}

template <typename T>
void SharedQueue<T>::processMessages() {
    while (true) {
        T message;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]() { return !message_queue_.empty() || stop_flag_; });

            if (stop_flag_ && message_queue_.empty())
                break;

            if (!message_queue_.empty()) {
                message = message_queue_.front();
                message_queue_.pop();
            }
        }

        if (subscriber_callback_) {
            subscriber_callback_(message);
        }
    }
}

#endif // SHAREDQUEUE_H
