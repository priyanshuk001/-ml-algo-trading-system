#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <httplib.h>

using json = nlohmann::json;

struct MLPrediction {
    int prediction;
    std::vector<double> probabilities;
    double score;
    std::string model_version;
    bool success;
    std::string error_message;
    
    MLPrediction()
        : prediction(0), score(0.0), success(false) {}
};

class MLClient {
private:
    std::string host_;
    int port_;
    httplib::Client client_;
    
public:
    MLClient(const std::string& host = "127.0.0.1", int port = 8000)
        : host_(host), port_(port), client_(host_ + ":" + std::to_string(port_)) {
        client_.set_connection_timeout(5, 0);  // 5 seconds
        client_.set_read_timeout(10, 0);       // 10 seconds
    }
    
    bool check_health() {
        auto res = client_.Get("/health");
        
        if (!res) {
            printf("[ML] Health check failed: Connection error\n");
            return false;
        }
        
        if (res->status != 200) {
            printf("[ML] Health check failed: HTTP %d\n", res->status);
            return false;
        }
        
        try {
            json response = json::parse(res->body);
            bool model_loaded = response["model_loaded"].get<bool>();
            
            if (!model_loaded) {
                printf("[ML] Health check: Model not loaded\n");
                return false;
            }
            
            printf("[ML] Health check: OK\n");
            return true;
        }
        catch (const std::exception& e) {
            printf("[ML] Health check parse error: %s\n", e.what());
            return false;
        }
    }
    
    MLPrediction predict(const std::string& symbol, std::time_t timestamp,
                         const std::vector<double>& features) {
        MLPrediction result;
        
        // Build JSON request
        json request = {
            {"symbol", symbol},
            {"timestamp", static_cast<long long>(timestamp)},
            {"features", features}
        };
        
        std::string request_body = request.dump();
        
        // Make POST request
        auto res = client_.Post("/predict", request_body, "application/json");
        
        if (!res) {
            result.error_message = "Connection failed";
            result.success = false;
            printf("[ML] Prediction failed: Connection error\n");
            return result;
        }
        
        if (res->status != 200) {
            result.error_message = "HTTP " + std::to_string(res->status);
            result.success = false;
            printf("[ML] Prediction failed: HTTP %d\n", res->status);
            return result;
        }
        
        try {
            json response = json::parse(res->body);
            
            result.prediction = response["prediction"].get<int>();
            result.probabilities = response["probabilities"].get<std::vector<double>>();
            result.score = response["score"].get<double>();
            result.model_version = response["model_version"].get<std::string>();
            result.success = true;
            
        }
        catch (const std::exception& e) {
            result.error_message = std::string("Parse error: ") + e.what();
            result.success = false;
            printf("[ML] Parse error: %s\n", e.what());
        }
        
        return result;
    }
};
