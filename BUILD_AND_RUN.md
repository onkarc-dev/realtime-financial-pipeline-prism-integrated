# Build and Run Guide

This cleaned production-style project has 3 modes.

## Mode 1: Benchmark Engine
No Kafka, no Docker, no Binance. Use this first.

```powershell
cd C:\Users\Admin\realtime-financial-pipeline
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
.\Release\benchmark_engine.exe 10000000 1000000
```

Meaning:
- `10000000` = total synthetic messages
- `1000000` = target rate: 1M msg/sec

Run unlimited stress mode:

```powershell
.\Release\benchmark_engine.exe 10000000
```

## Mode 2: Live Binance Engine
Requires libwebsockets.

Install dependency with vcpkg:

```powershell
cd C:\vcpkg
.\vcpkg install libwebsockets:x64-windows
```

Build with vcpkg toolchain:

```powershell
cd C:\Users\Admin\realtime-financial-pipeline
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

Run live BTCUSDT trade stream:

```powershell
.\Release\live_binance_engine.exe btcusdt
```

Run ETHUSDT:

```powershell
.\Release\live_binance_engine.exe ethusdt
```

Output example:

```text
live_rate_msg_s=123 total_processed=12345 ws_received=12345 ws_dropped=0 last_price=65000.12 max_engine_latency_ns=400
```

## Mode 3: Kafka Demo
Requires Docker Kafka plus protobuf and librdkafka.

Install dependencies:

```powershell
cd C:\vcpkg
.\vcpkg install protobuf librdkafka:x64-windows
```

Start Kafka:

```powershell
cd C:\Users\Admin\realtime-financial-pipeline
docker compose up -d
```

Build with vcpkg toolchain, then run:

```powershell
.\Release\kafka_consumer.exe localhost:9092 raw-trades
.\Release\kafka_producer.exe localhost:9092 raw-trades 100000
```

Kafka is not the lowest-latency hot path. It is for demo, analytics, and durable streaming.
