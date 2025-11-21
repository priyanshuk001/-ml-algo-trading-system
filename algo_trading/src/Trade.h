#pragma once

#include <string>
#include <ctime>

struct Trade {
    std::time_t timestamp;
    std::string strategy;
    std::string symbol;
    std::string side;  // "BUY" or "SELL"
    int quantity;
    double price;
    double cash_after;
    int position_after;
    
    // ML prediction data
    int ml_prediction;
    double ml_score;
    double ml_prob_buy;
    std::string model_version;
    
    Trade()
        : timestamp(0), strategy(""), symbol(""), side(""),
          quantity(0), price(0), cash_after(0), position_after(0),
          ml_prediction(0), ml_score(0), ml_prob_buy(0), model_version("") {}
    
    Trade(std::time_t ts, const std::string& strat, const std::string& sym,
          const std::string& sd, int qty, double px, double cash, int pos,
          int ml_pred, double ml_sc, double ml_pb, const std::string& mv)
        : timestamp(ts), strategy(strat), symbol(sym), side(sd),
          quantity(qty), price(px), cash_after(cash), position_after(pos),
          ml_prediction(ml_pred), ml_score(ml_sc), ml_prob_buy(ml_pb),
          model_version(mv) {}
};
