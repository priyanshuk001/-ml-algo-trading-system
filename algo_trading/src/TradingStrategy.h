#pragma once

#include "MarketDataEvent.h"
#include "Portfolio.h"
#include <memory>
#include <string>

class TradingStrategy {
protected:
    std::shared_ptr<Portfolio> portfolio_;
    std::string name_;
    
public:
    TradingStrategy(std::shared_ptr<Portfolio> portfolio, const std::string& name)
        : portfolio_(portfolio), name_(name) {}
    
    virtual ~TradingStrategy() = default;
    
    virtual void on_market_data(const MarketDataEvent& event) = 0;
    
    std::string get_name() const {
        return name_;
    }
};
