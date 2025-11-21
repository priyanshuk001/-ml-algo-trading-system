#include "MarketDataEvent.h"
#include "BacktestingEngine.h"
#include "Portfolio.h"
#include "TradeLogger.h"
#include "TradingStrategy.h"
#include "MovingAverageStrategy.h"
#include "Utils.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    printf("\n");
    printf("========================================\n");
    printf("  ALGORITHMIC TRADING BACKTESTER\n");
    printf("  ML-Integrated System\n");
    printf("========================================\n\n");
    
    // Configuration
    const std::string data_file = "data/sample_AAPL.csv";
    const std::string trades_file = "trades.csv";
    const double initial_cash = 10000.0;
    
    // Step 1: Load market data
    printf("[1/5] Loading market data...\n");
    auto events = Utils::load_csv(data_file);
    
    if (events.empty()) {
        printf("[ERROR] No events loaded. Exiting.\n");
        return 1;
    }
    
    printf("[INFO] Loaded %zu events\n\n", events.size());
    
    // Step 2: Create components
    printf("[2/5] Initializing components...\n");
    
    auto logger = std::make_shared<TradeLogger>();
    auto portfolio = std::make_shared<Portfolio>(initial_cash, logger);
    
    auto strategy = std::make_shared<MovingAverageStrategy>(
        portfolio,
        10,   // Short MA period
        50,   // Long MA period
        0.7   // ML confidence threshold
    );
    
    auto engine = std::make_unique<BacktestingEngine>(strategy);
    
    printf("[INFO] Components initialized\n\n");
    
    // Step 3: Start engine
    printf("[3/5] Starting backtesting engine...\n");
    engine->start();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    printf("[INFO] Engine started\n\n");
    
    // Step 4: Feed events
    printf("[4/5] Processing market events...\n");
    printf("========================================\n\n");
    int count = 0;
    for (const auto& event : events) {
        if (count++ >= 200) break;  // Only 100 events
        engine->add_event(event);
        
        // Small delay to simulate real-time (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    printf("\n========================================\n");
    printf("[INFO] All events processed\n\n");
    
    // Step 5: Stop engine and save results
    printf("[5/5] Finalizing...\n");
    engine->stop();
    
    // Wait for engine to fully stop
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Save trades to CSV
    logger->save_to_csv(trades_file);
    
    // Print portfolio summary
    std::map<std::string, double> final_prices;
    if (!events.empty()) {
        final_prices[events.back().symbol] = events.back().close;
    }
    
    printf("\n");
    portfolio->print_summary(final_prices);
    
    printf("\n========================================\n");
    printf("  BACKTESTING COMPLETE!\n");
    printf("========================================\n");
    printf("\nResults saved to: %s\n", trades_file.c_str());
    printf("Total trades executed: %zu\n\n", logger->count());
    
    return 0;
}
