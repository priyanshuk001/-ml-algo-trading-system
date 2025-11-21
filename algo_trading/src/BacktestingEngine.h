#pragma once

#include "MarketDataEvent.h"
#include "ThreadSafeQueue.h"
#include "TradingStrategy.h"
#include <thread>
#include <memory>
#include <atomic>

class BacktestingEngine {
private:
    ThreadSafeQueue<MarketDataEvent> event_queue_;
    std::shared_ptr<TradingStrategy> strategy_;
    std::thread processing_thread_;
    std::atomic<bool> running_;
    
public:
    BacktestingEngine(std::shared_ptr<TradingStrategy> strategy)
        : strategy_(strategy), running_(false) {}
    
    ~BacktestingEngine() {
        stop();
    }
    
    void add_event(const MarketDataEvent& event) {
        event_queue_.push(event);
    }
    
    void start() {
        running_ = true;
        
        processing_thread_ = std::thread([this]() {
            printf("[INFO] Backtesting engine started\n");
            
            while (running_) {
                auto event = event_queue_.pop();
                
                if (!event.has_value()) {
                    break;  // Queue finished
                }
                
                strategy_->on_market_data(event.value());
            }
            
            printf("[INFO] Backtesting engine stopped\n");
        });
    }
    
    void stop() {
        if (running_) {
            running_ = false;
            event_queue_.finish();
            
            if (processing_thread_.joinable()) {
                processing_thread_.join();
            }
        }
    }
    
    bool is_running() const {
        return running_;
    }
};
