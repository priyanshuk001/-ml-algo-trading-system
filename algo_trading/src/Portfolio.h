#pragma once

#include "Trade.h"
#include "TradeLogger.h"
#include <string>
#include <map>
#include <memory>

class Portfolio {
private:
    double initial_cash_;
    double cash_;
    std::map<std::string, int> positions_;
    std::shared_ptr<TradeLogger> logger_;
    
public:
    Portfolio(double initial_cash, std::shared_ptr<TradeLogger> logger)
        : initial_cash_(initial_cash), cash_(initial_cash), logger_(logger) {}
    
    bool can_buy(const std::string& symbol, int quantity, double price) const {
        double cost = quantity * price;
        return cash_ >= cost;
    }
    
    bool can_sell(const std::string& symbol, int quantity) const {
        auto it = positions_.find(symbol);
        if (it == positions_.end()) return false;
        return it->second >= quantity;
    }
    
    void execute_trade(std::time_t timestamp, const std::string& strategy,
                       const std::string& symbol, const std::string& side,
                       int quantity, double price,
                       int ml_prediction, double ml_score, double ml_prob_buy,
                       const std::string& model_version) {
        
        if (side == "BUY") {
            double cost = quantity * price;
            cash_ -= cost;
            positions_[symbol] += quantity;
            
            printf("[TRADE] BUY %d %s @ $%.2f (Cash: $%.2f)\n",
                   quantity, symbol.c_str(), price, cash_);
        }
        else if (side == "SELL") {
            double proceeds = quantity * price;
            cash_ += proceeds;
            positions_[symbol] -= quantity;
            
            if (positions_[symbol] == 0) {
                positions_.erase(symbol);
            }
            
            printf("[TRADE] SELL %d %s @ $%.2f (Cash: $%.2f)\n",
                   quantity, symbol.c_str(), price, cash_);
        }
        
        // Log trade
        Trade trade(timestamp, strategy, symbol, side, quantity, price,
                    cash_, get_position(symbol),
                    ml_prediction, ml_score, ml_prob_buy, model_version);
        logger_->log_trade(trade);
    }
    
    double get_cash() const {
        return cash_;
    }
    
    int get_position(const std::string& symbol) const {
        auto it = positions_.find(symbol);
        return (it != positions_.end()) ? it->second : 0;
    }
    
    double get_total_value(const std::map<std::string, double>& prices) const {
        double value = cash_;
        
        for (const auto& [symbol, quantity] : positions_) {
            auto it = prices.find(symbol);
            if (it != prices.end()) {
                value += quantity * it->second;
            }
        }
        
        return value;
    }
    
    void print_summary(const std::map<std::string, double>& prices) const {
        printf("\n=== PORTFOLIO SUMMARY ===\n");
        printf("Initial Cash: $%.2f\n", initial_cash_);
        printf("Current Cash: $%.2f\n", cash_);
        printf("\nPositions:\n");
        
        if (positions_.empty()) {
            printf("  (No positions)\n");
        } else {
            for (const auto& [symbol, quantity] : positions_) {
                auto it = prices.find(symbol);
                double price = (it != prices.end()) ? it->second : 0.0;
                double value = quantity * price;
                printf("  %s: %d shares @ $%.2f = $%.2f\n",
                       symbol.c_str(), quantity, price, value);
            }
        }
        
        double total_value = get_total_value(prices);
        double pnl = total_value - initial_cash_;
        double pnl_pct = (pnl / initial_cash_) * 100.0;
        
        printf("\nTotal Value: $%.2f\n", total_value);
        printf("P&L: $%.2f (%.2f%%)\n", pnl, pnl_pct);
        printf("=========================\n");
    }
};
