#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

template <typename T>
class SafeQueue {
public:
    explicit SafeQueue(size_t max_size = 5)
        : max_size_(max_size), running_(true) {}

    // Disable copy
    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;

    void push(T item) {
        std::scoped_lock lock(mtx_);
        if (!running_) return;

        if (queue_.size() >= max_size_) {
            queue_.pop();
        }

        queue_.push(std::move(item));
        cond_.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cond_.wait(lock, [this] { return !queue_.empty() || !running_; });

        if (queue_.empty()) {
            return std::nullopt;
        }

        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    void request_stop() {
        running_ = false;
        cond_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mtx_;
    std::condition_variable cond_;
    const size_t max_size_;
    std::atomic<bool> running_;
};