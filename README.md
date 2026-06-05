# Realtime Financial Pipeline — PRISM Integrated

A low-latency C++ realtime financial market data pipeline integrated with a PRISM-style breakout/retest strategy engine, Binance WebSocket live feed, lock-free queueing, trade simulation, CSV audit logging, and JSON performance reporting.

## Overview

This project demonstrates a realtime quant systems engineering pipeline:

```text
Binance WebSocket Feed
        ↓
TradePacket Parser
        ↓
Lock-Free SPSC Queue
        ↓
Live Engine Thread
        ↓
Order Book / Bar Builder
        ↓
PRISM Strategy Engine
        ↓
Trade Manager
        ↓
CSV Trade Log + JSON Performance Summary
```

The goal is not to claim profitability. The goal is to demonstrate deterministic, auditable, low-latency trading-system architecture.

## Key Features

* Live Binance WebSocket trade feed
* Modern C++17 implementation
* Lock-free single-producer/single-consumer queue
* Multithreaded feed and processing pipeline
* PRISM breakout/retest strategy engine
* Realtime OHLCV bar generation
* ATR-aware stop-loss calculation
* Trade manager with entry, stop-loss, target1, target2, and breakeven logic
* CSV trade audit log
* JSON performance summary
* CMake-based build system
* vcpkg dependency integration
* Windows/MSVC compatible

## Technology Stack

* C++17
* CMake
* MSVC / Visual Studio Build Tools
* libwebsockets
* Protocol Buffers
* librdkafka
* vcpkg
* Binance WebSocket API

## Core Modules

| Module                    | Description                                                  |
| ------------------------- | ------------------------------------------------------------ |
| `BinanceClient.cpp`       | Connects to Binance WebSocket and parses live trade messages |
| `engine_shared.hpp`       | Defines `TradePacket` and lock-free SPSC queue               |
| `live_binance_engine.cpp` | Main realtime engine loop                                    |
| `order_book.hpp`          | Lightweight order book / price processing module             |
| `prism_live_engine.cpp`   | Builds live OHLCV bars and routes data to strategy           |
| `prism_strategy.cpp`      | Breakout/retest PRISM strategy logic                         |
| `trade_manager.cpp`       | Trade lifecycle, SL/targets, CSV logging, JSON summary       |

## Build Instructions

### 1. Clone repository

```bash
git clone https://github.com/onkarc-dev/realtime-financial-pipeline-prism-integrated.git
cd realtime-financial-pipeline-prism-integrated
```

### 2. Configure CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### 3. Build

```bash
cmake --build . --config Release
```

### 4. Run live Binance engine

```bash
Release\live_binance_engine.exe btcusdt
```

## Runtime Outputs

The engine generates:

```text
trade_log.csv
performance_summary.json
```

Example status line:

```text
live_rate_msg_s=250 total_processed=50000 ws_received=50000 ws_dropped=1200 last_price=59700 bars=35 prism_signals_total=1 open_trade=0 total_trades=1 wins=0 losses=1 breakevens=0 gross_R=-1.01 avg_R=-1.01 max_engine_latency_ns=17945400
```

## PRISM Strategy Logic

The PRISM module currently implements a simplified live-testing strategy:

1. Build live OHLCV bars from Binance trade feed.
2. Detect breakout above recent high.
3. Wait for retest near breakout level.
4. Score the setup using close position, candle direction, and volume.
5. Generate trade signal if score passes threshold.
6. TradeManager handles stop-loss, targets, and breakeven logic.

## Current Project Status

This project is a quant systems engineering prototype.

| Level                          | Status                  |
| ------------------------------ | ----------------------- |
| College / Portfolio Project    | Strong                  |
| Junior Quant Dev Project       | Good                    |
| Startup Prototype              | Moderate                |
| Production Prop Trading System | Not production-ready    |
| Institutional HFT System       | Research prototype only |

## Important Disclaimer

This project is for educational, research, and engineering demonstration purposes only. It is not financial advice and should not be used for live trading without extensive validation, risk controls, backtesting, monitoring, and compliance review.

## Author

Onkar Chougule
GitHub: [onkarc-dev](https://github.com/onkarc-dev)
