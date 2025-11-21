Full project zip contents:
- backend_python/: Python training + FastAPI server
- algo_trading/: C++ backtesting engine (CMake)
How to run:
1) Start Python server:
   cd backend_python
   python -m venv .venv
   .\.venv\Scripts\activate
   pip install -r requirements.txt
   python train_model.py   # optional to create model.pkl
   uvicorn main_ml_api:app --host 127.0.0.1 --port 8000 --reload

2) Build C++:
   cd algo_trading
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/project/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release

3) Run backtester:
   .\bin\backtester.exe ..\data\sample_AAPL.csv

Note: ensure vcpkg has nlohmann-json and cpp-httplib installed.
