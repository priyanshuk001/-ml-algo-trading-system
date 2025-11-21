#pragma once

#include "TradingStrategy.h"
#include "ml_client.h"
#include <deque>
#include <numeric>
#include <memory>

class MovingAverageStrategy : public TradingStrategy {
private:
    int short_period_;
    int long_period_;
    double ml_threshold_;
    
    std::deque<double> price_history_;
    std::deque<double> volume_history_;
    std::unique_ptr<MLClient> ml_client_;
    
    double prev_close_;
    bool initialized_;
    
public:
    MovingAverageStrategy(std::shared_ptr<Portfolio> portfolio,
                          int short_period = 10,
                          int long_period = 50,
                          double ml_threshold = 0.7)
        : TradingStrategy(portfolio, "MovingAverage"),
          short_period_(short_period),
          long_period_(long_period),
          ml_threshold_(ml_threshold),
          prev_close_(0),
          initialized_(false) {
        
        ml_client_ = std::make_unique<MLClient>("127.0.0.1", 8000);

        
        // Check ML server health
        if (!ml_client_->check_health()) {
            printf("[WARNING] ML server not available. Strategy will not work!\n");
        }
    }
    
    void on_market_data(const MarketDataEvent& event) override {
        // Add to history
        price_history_.push_back(event.close);
        volume_history_.push_back(static_cast<double>(event.volume));
        
        // Keep history size manageable
        if (price_history_.size() > static_cast<size_t>(long_period_ + 10)) {
            price_history_.pop_front();
            volume_history_.pop_front();
        }
        
        // Need enough data for long MA
        if (price_history_.size() < static_cast<size_t>(long_period_)) {
            prev_close_ = event.close;
            return;
        }
        
        if (!initialized_) {
            initialized_ = true;
            prev_close_ = event.close;
            return;
        }
        
        // Calculate features
        double return_1 = (event.close - prev_close_) / prev_close_;
        
        double short_ma = calculate_ma(short_period_);
        double long_ma = calculate_ma(long_period_);
        
        double volatility = calculate_volatility(20);
        double volume_ratio = calculate_volume_ratio(20);
        
        double momentum = (event.close - price_history_[price_history_.size() - 5]) / 
                         price_history_[price_history_.size() - 5];
        
        double return_5 = (event.close - price_history_[price_history_.size() - 5]) / 
                         price_history_[price_history_.size() - 5];
        
        // Prepare feature vector for ML model
        std::vector<double> features = {
            return_1,
            return_5,
            short_ma,
            long_ma,
            volatility,
            volume_ratio,
            event.close,
            momentum
        };
        
        // Call ML model
        printf("[ML] Calling prediction for %s...\n", event.symbol.c_str());
        MLPrediction ml_pred = ml_client_->predict(event.symbol, event.timestamp, features);
        
        if (!ml_pred.success) {
            printf("[ML] Prediction failed: %s\n", ml_pred.error_message.c_str());
            prev_close_ = event.close;
            return;
        }
        
        printf("[ML] Prediction: %d, Score: %.4f, Prob[BUY]: %.4f\n",
               ml_pred.prediction, ml_pred.score, ml_pred.probabilities[1]);
        
        // Trading logic: Use ML prediction + confidence threshold
        int position = portfolio_->get_position(event.symbol);
        
        // BUY signal: ML predicts BUY with high confidence
        if (ml_pred.prediction == 1 && ml_pred.score >= ml_threshold_ && position == 0) {
            int quantity = 10;  // Fixed quantity for simplicity
            
            if (portfolio_->can_buy(event.symbol, quantity, event.close)) {
                portfolio_->execute_trade(
                    event.timestamp, name_, event.symbol, "BUY",
                    quantity, event.close,
                    ml_pred.prediction, ml_pred.score, ml_pred.probabilities[1],
                    ml_pred.model_version
                );
            }
        }
        // SELL signal: ML predicts SELL with high confidence
        else if (ml_pred.prediction == 0 && ml_pred.score >= ml_threshold_ && position > 0) {
            int quantity = position;  // Sell all
            
            if (portfolio_->can_sell(event.symbol, quantity)) {
                portfolio_->execute_trade(
                    event.timestamp, name_, event.symbol, "SELL",
                    quantity, event.close,
                    ml_pred.prediction, ml_pred.score, ml_pred.probabilities[1],
                    ml_pred.model_version
                );
            }
        }
        
        prev_close_ = event.close;
    }
    
private:
    double calculate_ma(int period) {
        if (price_history_.size() < static_cast<size_t>(period)) {
            return 0.0;
        }
        
        auto start = price_history_.end() - period;
        auto end = price_history_.end();
        
        double sum = std::accumulate(start, end, 0.0);
        return sum / period;
    }
    
    double calculate_volatility(int period) {
        if (price_history_.size() < static_cast<size_t>(period)) {
            return 0.0;
        }
        
        auto start = price_history_.end() - period;
        auto end = price_history_.end();
        
        double mean = std::accumulate(start, end, 0.0) / period;
        
        double sq_sum = 0.0;
        for (auto it = start; it != end; ++it) {
            sq_sum += (*it - mean) * (*it - mean);
        }
        
        return std::sqrt(sq_sum / period);
    }
    
    double calculate_volume_ratio(int period) {
        if (volume_history_.size() < static_cast<size_t>(period + 1)) {
            return 1.0;
        }
        
        double current_volume = volume_history_.back();
        
        auto start = volume_history_.end() - period - 1;
        auto end = volume_history_.end() - 1;
        
        double avg_volume = std::accumulate(start, end, 0.0) / period;
        
        return (avg_volume > 0) ? (current_volume / avg_volume) : 1.0;
    }
};
