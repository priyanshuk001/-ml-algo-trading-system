#pragma once

#include "MarketDataEvent.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

class Utils {
public:
    static std::time_t parse_timestamp(const std::string& timestamp_str) {
        // Simple parser for ISO format: 2020-01-02T09:30:00Z
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        
        // Parse YYYY-MM-DDTHH:MM:SSZ
        char dash1, dash2, T, colon1, colon2, Z;
        ss >> tm.tm_year >> dash1 >> tm.tm_mon >> dash2 >> tm.tm_mday
           >> T >> tm.tm_hour >> colon1 >> tm.tm_min >> colon2 >> tm.tm_sec >> Z;
        
        tm.tm_year -= 1900;  // Years since 1900
        tm.tm_mon -= 1;      // Months since January
        tm.tm_isdst = 0;
        
        return std::mktime(&tm);
    }
    
    static std::vector<MarketDataEvent> load_csv(const std::string& filename) {
        std::vector<MarketDataEvent> events;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            printf("[ERROR] Could not open file: %s\n", filename.c_str());
            return events;
        }
        
        std::string line;
        std::getline(file, line);  // Skip header
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string timestamp_str, symbol, open_str, high_str, low_str,
                        close_str, adj_close_str, volume_str, bid_str, ask_str;
            
            std::getline(ss, timestamp_str, ',');
            std::getline(ss, symbol, ',');
            std::getline(ss, open_str, ',');
            std::getline(ss, high_str, ',');
            std::getline(ss, low_str, ',');
            std::getline(ss, close_str, ',');
            std::getline(ss, adj_close_str, ',');
            std::getline(ss, volume_str, ',');
            std::getline(ss, bid_str, ',');
            std::getline(ss, ask_str, ',');
            
            try {
                std::time_t timestamp = parse_timestamp(timestamp_str);
                double open = std::stod(open_str);
                double high = std::stod(high_str);
                double low = std::stod(low_str);
                double close = std::stod(close_str);
                double adj_close = std::stod(adj_close_str);
                long long volume = std::stoll(volume_str);
                double bid = std::stod(bid_str);
                double ask = std::stod(ask_str);
                
                events.emplace_back(timestamp, symbol, open, high, low, close,
                                    adj_close, volume, bid, ask);
            }
            catch (const std::exception& e) {
                printf("[WARNING] Skipping invalid line: %s\n", line.c_str());
            }
        }
        
        file.close();
        printf("[INFO] Loaded %zu events from: %s\n", events.size(), filename.c_str());
        
        return events;
    }
};
