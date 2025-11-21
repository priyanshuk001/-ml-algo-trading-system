#pragma once

#include <string>
#include <ctime>

struct MarketDataEvent {
    std::time_t timestamp;
    std::string symbol;
    double open;
    double high;
    double low;
    double close;
    double adj_close;
    long long volume;
    double bid;
    double ask;
    
    MarketDataEvent()
        : timestamp(0), symbol(""), open(0), high(0), low(0),
          close(0), adj_close(0), volume(0), bid(0), ask(0) {}
    
    MarketDataEvent(std::time_t ts, const std::string& sym,
                    double o, double h, double l, double c,
                    double ac, long long vol, double b, double a)
        : timestamp(ts), symbol(sym), open(o), high(h), low(l),
          close(c), adj_close(ac), volume(vol), bid(b), ask(a) {}
    
    double mid_price() const {
        return (bid + ask) / 2.0;
    }
    
    double spread() const {
        return ask - bid;
    }
    
    void print() const {
        printf("[Event] %s @ %ld: O=%.2f H=%.2f L=%.2f C=%.2f V=%lld\n",
               symbol.c_str(), timestamp, open, high, low, close, volume);
    }
};
