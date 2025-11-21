# 🤖 ML-Integrated Algorithmic Trading System

![Python](https://img.shields.io/badge/Python-3.10+-blue.svg)
![C++](https://img.shields.io/badge/C++-17-orange.svg)
![Status](https://img.shields.io/badge/Status-Production-brightgreen.svg)

> **Production-grade algorithmic trading system** combining Machine Learning predictions (Python) with high-performance backtesting (C++17) through REST API architecture.

---

## 📊 Project Overview

A complete **end-to-end algorithmic trading system** that demonstrates real-world software engineering practices:

- **ML Backend**: Python FastAPI server serving Random Forest predictions
- **Trading Engine**: C++ event-driven backtester with multithreading
- **Real Data**: Trained on 1,510 Apple Inc. stock data points
- **Production Architecture**: Microservices with HTTP communication

### 🎯 Key Achievements

| Metric | Value |
|--------|-------|
| **Model Accuracy** | 63.91% on real market data |
| **Processing Speed** | ~50-100ms per event |
| **Data Points** | 1,510 AAPL records |
| **Architecture** | Client-Server (REST API) |

---

## ✨ Features

### 🐍 Python ML Backend
- ✅ **Random Forest Classifier** trained on real stock data
- ✅ **FastAPI REST API** for real-time predictions
- ✅ **Automated feature engineering** (returns, MAs, volatility, momentum)
- ✅ **Label generation** based on future price movements
- ✅ **Model persistence** with joblib

### 🚀 C++ Backtesting Engine
- ✅ **Event-driven architecture** for realistic market simulation
- ✅ **Multithreaded processing** with thread-safe queues
- ✅ **HTTP client** for ML predictions (cpp-httplib)
- ✅ **JSON handling** (nlohmann-json)
- ✅ **Portfolio management** with P&L tracking
- ✅ **Trade logging** to CSV with full audit trail

### 📈 Trading Strategy
- ✅ **Moving Average crossover** with ML enhancement
- ✅ **Confidence-based filtering** (threshold: 70%)
- ✅ **Risk management** (position checks, cash management)
- ✅ **Multiple technical indicators** (volatility, volume ratio)

---

## 🛠️ Tech Stack

### Backend
- **Language**: Python 3.10+
- **ML Framework**: scikit-learn 1.3.2
- **Web Framework**: FastAPI 0.104.1
- **Server**: Uvicorn
- **Data Processing**: pandas, numpy

### Backtester
- **Language**: C++17
- **Build System**: CMake 3.15+
- **Package Manager**: vcpkg
- **Dependencies**:
  - nlohmann-json (JSON parsing)
  - cpp-httplib (HTTP client)
  - Standard C++17 threading library

---

## 🚀 Quick Start

### Prerequisites

- **Python 3.10+** ([Download](https://www.python.org/downloads/))
- **CMake 3.15+** ([Download](https://cmake.org/download/))
- **vcpkg** ([Installation Guide](https://vcpkg.io/en/getting-started.html))
- **C++ Compiler** (MSVC 2019+, GCC 9+, or Clang 10+)


---

## 📊 Results & Analysis

After running the backtester, check:

### `trades.csv`
Complete trade log with:
- Timestamp
- Symbol (AAPL)
- Side (BUY/SELL)
- Quantity & Price
- ML Prediction & Confidence
- Portfolio state after trade

### Portfolio Summary
Initial Cash: $10000.00
Current Cash: $10041.62
Total Value: $10041.62
P&L: $41.62 (0.42%)



---

## 🎓 Learning Outcomes

This project demonstrates:

✅ **ML Model Deployment** via REST API  
✅ **Microservices Architecture** (Python ↔ C++)  
✅ **Modern C++17** (smart pointers, templates, RAII)  
✅ **Multithreading** (producer-consumer pattern)  
✅ **Event-Driven Systems** (backtesting simulation)  
✅ **HTTP Client-Server** communication  
✅ **Financial data processing** & feature engineering  
✅ **Professional project structure** & build systems

---


---

## 🚀 Future Enhancements

- [ ] **More Indicators**: RSI, MACD, Bollinger Bands
- [ ] **Advanced ML**: XGBoost, LSTM neural networks
- [ ] **Risk Management**: Stop-loss, position sizing, drawdown limits
- [ ] **Multiple Assets**: Multi-symbol portfolio
- [ ] **Database Integration**: PostgreSQL for data storage
- [ ] **Web Dashboard**: React frontend for visualization
- [ ] **Live Trading**: Broker API integration (Interactive Brokers, Alpaca)
- [ ] **Backtesting Metrics**: Sharpe ratio, max drawdown, win rate
- [ ] **Docker**: Containerized deployment
- [ ] **CI/CD**: Automated testing & deployment

---


## 📧 Contact

**Author:** [Priyanshu Kumar]  
**GitHub:** [@priyanshuk001](https://github.com/priyanshuk001)  
**Project Link:** https://github.com/priyanshuk001/ml-algo-trading-system

---

## 🙏 Acknowledgments

- **scikit-learn** - ML framework
- **FastAPI** - Modern Python web framework
- **nlohmann-json** - C++ JSON library
- **cpp-httplib** - C++ HTTP client
- **CMake** - Build system
- **vcpkg** - C++ package manager

---

**⭐ If you found this project helpful, please give it a star!**

**Built with ❤️ for algorithmic trading and software engineering enthusiasts**



