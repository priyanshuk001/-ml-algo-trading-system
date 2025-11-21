#pragma once

#include "Trade.h"
#include <fstream>
#include <vector>
#include <string>

class TradeLogger {
private:
    std::vector<Trade> trades_;
    
public:
    void log_trade(const Trade& trade) {
        trades_.push_back(trade);
    }
    
    void save_to_csv(const std::string& filename) {
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            printf("[ERROR] Could not open file: %s\n", filename.c_str());
            return;
        }
        
        // Write header
        file << "timestamp,strategy,symbol,side,qty,price,cash_after,position_after,"
             << "ml_prediction,ml_score,ml_prob_buy,model_version\n";
        
        // Write trades
        for (const auto& trade : trades_) {
            file << trade.timestamp << ","
                 << trade.strategy << ","
                 << trade.symbol << ","
                 << trade.side << ","
                 << trade.quantity << ","
                 << trade.price << ","
                 << trade.cash_after << ","
                 << trade.position_after << ","
                 << trade.ml_prediction << ","
                 << trade.ml_score << ","
                 << trade.ml_prob_buy << ","
                 << trade.model_version << "\n";
        }
        
        file.close();
        printf("[INFO] Saved %zu trades to: %s\n", trades_.size(), filename.c_str());
    }
    
    size_t count() const {
        return trades_.size();
    }
    
    const std::vector<Trade>& get_trades() const {
        return trades_;
    }
};
