#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
    bool finished_ = false;
    
public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
        }
        cond_var_.notify_one();
    }
    
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        cond_var_.wait(lock, [this] {
            return !queue_.empty() || finished_;
        });
        
        if (finished_ && queue_.empty()) {
            return std::nullopt;
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.empty()) {
            return std::nullopt;
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    void finish() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            finished_ = true;
        }
        cond_var_.notify_all();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    bool is_finished() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return finished_;
    }
};
