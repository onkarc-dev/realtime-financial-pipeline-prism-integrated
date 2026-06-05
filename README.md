# Realtime Financial Pipeline — PRISM Integrated

A low-latency C++ real-time financial market data pipeline integrating live Binance market feeds, lock-free queueing, PRISM breakout/retest strategy evaluation, trade lifecycle management, performance analytics, and audit reporting.

The project is designed as a quantitative systems engineering prototype focused on deterministic processing, low-latency architecture, auditability, and extensibility toward production-grade trading infrastructure.

---

## Overview

This project demonstrates the core building blocks commonly found in modern quantitative trading systems:

```text
Binance WebSocket Feed
          │
          ▼
Trade Message Parser
          │
          ▼
Lock-Free SPSC Queue
          │
          ▼
Live Processing Engine
          │
 ┌────────┴────────┐
 ▼                 ▼
Order Book     OHLCV Builder
                    │
                    ▼
            PRISM Strategy Engine
                    │
                    ▼
              Trade Manager
                    │
                    ▼
      CSV Audit Logs + JSON Reports
```

The primary objective is not profitability testing, but the implementation of a deterministic, auditable, low-latency market data and strategy processing pipeline.

---

## Key Features

### Market Data Infrastructure

* Live Binance WebSocket market data ingestion
* Real-time trade parsing and normalization
* Low-overhead market data processing
* Multithreaded feed and processing architecture

### Performance Engineering

* Lock-free Single Producer Single Consumer (SPSC) queue
* Cache-friendly data structures
* Low-latency event processing
* Runtime latency monitoring
* Throughput metrics and diagnostics

### Strategy Engine

* PRISM breakout/retest strategy implementation
* Dynamic OHLCV bar generation
* Setup scoring framework
* Signal generation pipeline
* ATR-aware trade management

### Trade Lifecycle Management

* Automated entry generation
* Stop-loss management
* Multi-target exits
* Breakeven handling
* Trade state tracking

### Analytics & Reporting

* CSV trade audit logs
* JSON performance summaries
* Win/loss tracking
* R-multiple calculations
* Strategy performance monitoring

### Engineering Tooling

* Modern C++17 implementation
* CMake build system
* vcpkg package management
* Windows/MSVC support
* Modular architecture for future expansion

---

## Technology Stack

| Category        | Technologies                               |
| --------------- | ------------------------------------------ |
| Language        | C++17                                      |
| Build System    | CMake                                      |
| Compiler        | MSVC                                       |
| Package Manager | vcpkg                                      |
| Networking      | libwebsockets                              |
| Messaging       | Protocol Buffers                           |
| Streaming       | Apache Kafka (librdkafka)                  |
| Market Data     | Binance WebSocket API                      |
| Concurrency     | std::thread, atomics, lock-free structures |

---

## Core Modules

| Module                    | Description                                           |
| ------------------------- | ----------------------------------------------------- |
| `BinanceClient.cpp`       | Live Binance WebSocket connectivity and trade parsing |
| `engine_shared.hpp`       | Trade packet definitions and lock-free queue          |
| `live_binance_engine.cpp` | Main processing engine                                |
| `order_book.hpp`          | Lightweight order book processing                     |
| `prism_live_engine.cpp`   | OHLCV generation and strategy routing                 |
| `prism_strategy.cpp`      | PRISM breakout/retest logic                           |
| `trade_manager.cpp`       | Trade execution simulation and lifecycle management   |
| `producer.cpp`            | Kafka producer implementation                         |
| `consumer.cpp`            | Kafka consumer implementation                         |

---

## System Architecture

```text
┌──────────────────────────┐
│ Binance WebSocket Feed   │
└──────────────┬───────────┘
               │
               ▼
┌──────────────────────────┐
│ Trade Message Parser     │
└──────────────┬───────────┘
               │
               ▼
┌──────────────────────────┐
│ Lock-Free SPSC Queue     │
└──────────────┬───────────┘
               │
               ▼
┌──────────────────────────┐
│ Live Processing Engine   │
└───────┬──────────┬───────┘
        │          │
        ▼          ▼
┌───────────┐  ┌───────────┐
│Order Book │  │OHLCV Bars │
└─────┬─────┘  └─────┬─────┘
      │              │
      └──────┬───────┘
             ▼
┌──────────────────────────┐
│ PRISM Strategy Engine    │
└──────────────┬───────────┘
               ▼
┌──────────────────────────┐
│ Trade Manager            │
└──────────────┬───────────┘
               ▼
┌──────────────────────────┐
│ CSV + JSON Reporting     │
└──────────────────────────┘
```

---

## Build Instructions

### Clone Repository

```bash
git clone https://github.com/onkarc-dev/realtime-financial-pipeline-prism-integrated.git
cd realtime-financial-pipeline-prism-integrated
```

### Configure Build

```bash
mkdir build
cd build

cmake .. ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### Compile

```bash
cmake --build . --config Release
```

### Run Live Engine

```bash
Release\live_binance_engine.exe btcusdt
```

---

## Runtime Outputs

The system generates:

```text
trade_log.csv
performance_summary.json
```

Example runtime metrics:

```text
live_rate_msg_s=250
total_processed=50000
ws_received=50000
ws_dropped=1200
last_price=59700
bars=35
prism_signals_total=1
open_trade=0
total_trades=1
wins=0
losses=1
gross_R=-1.01
avg_R=-1.01
max_engine_latency_ns=17945400
```

---

## PRISM Strategy Logic

Current implementation:

1. Build OHLCV bars from incoming trades
2. Detect breakout above recent structure high
3. Wait for retest of breakout zone
4. Evaluate setup quality
5. Score the setup
6. Generate signal when threshold is exceeded
7. Create trade with stop-loss and targets
8. Manage trade lifecycle
9. Record performance metrics

---

## Roadmap & Future Enhancements

This repository is actively evolving toward a more production-oriented quantitative trading platform.

### Market Data Infrastructure

* Multi-exchange connectivity
* Coinbase integration
* Bybit integration
* OKX integration
* Historical data replay engine
* Unified market data abstraction layer

### Quantitative Research

* Multi-strategy framework
* Statistical arbitrage models
* Mean reversion models
* Momentum models
* Factor-based alpha generation
* Machine learning signal integration

### Execution & Risk

* Position sizing engine
* Portfolio-level risk controls
* Dynamic trailing stops
* Exposure management
* Drawdown protection
* Advanced execution simulator

### Performance Engineering

* Memory pool allocator
* NUMA-aware architecture
* CPU cache optimization
* Microsecond-level profiling
* Performance telemetry dashboard
* Benchmark suite

### Monitoring & Observability

* Prometheus integration
* Grafana dashboards
* Alerting framework
* Trade monitoring interface
* Health checks
* Centralized audit logging

### Analytics

* Portfolio analytics
* Monte Carlo simulation
* Walk-forward testing
* Advanced backtesting framework
* Risk-adjusted performance reporting
* Strategy attribution analysis

### Long-Term Vision

Transform the project into a modular quantitative trading infrastructure platform supporting:

* Market data processing
* Quantitative research
* Risk management
* Execution simulation
* Performance analytics
* Production-grade deployment workflows

---

## Maturity Assessment

| Capability Area             | Status         |
| --------------------------- | -------------- |
| Market Data Ingestion       | Implemented    |
| Live Strategy Evaluation    | Implemented    |
| Trade Lifecycle Management  | Implemented    |
| Performance Reporting       | Implemented    |
| Low-Latency Processing      | Implemented    |
| Risk Management             | Basic          |
| Backtesting Framework       | Partial        |
| Portfolio Analytics         | Planned        |
| Multi-Strategy Support      | Planned        |
| Production Deployment       | Future Roadmap |
| Institutional HFT Readiness | Research Stage |

---

## Highlights

* Built a low-latency C++ market data pipeline integrating Binance WebSocket feeds, lock-free queueing, real-time OHLCV generation, and multithreaded event processing.
* Developed a PRISM-style breakout/retest strategy engine with scoring, trade lifecycle management, stop-loss handling, and target-based exits.
* Implemented CSV audit logging, JSON performance reporting, runtime latency monitoring, and modular architecture suitable for quantitative systems engineering workflows.

---

## Important Disclaimer

This project is intended for educational, research, and engineering demonstration purposes only.

It does not constitute financial advice, investment advice, or a production-ready trading system. Live trading requires significantly more validation, testing, monitoring, compliance controls, and operational safeguards.

---

## Author

**Onkar Chougule**

GitHub: https://github.com/onkarc-dev

Project Type: Quantitative Systems Engineering / Low-Latency Trading Infrastructure Prototype
