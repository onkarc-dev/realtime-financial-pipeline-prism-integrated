# Fix Report

## Fixed modes

### 1. benchmark_engine
Status: builds and runs without external dependencies.

Verified in sandbox:
- 100,000 messages
- target rate: 1,000,000 msg/sec
- observed throughput: ~981,948 msg/sec
- p99 latency: ~7,297 ns = 0.007297 ms

Your laptop result may differ depending on CPU, power mode, antivirus, and background processes.

### 2. live_binance_engine
Status: source code added and CMake target enabled when libwebsockets is installed.

Files added/updated:
- cpp_engine/src/live_binance_engine.cpp
- cpp_engine/src/connectors/BinanceClient.h
- cpp_engine/src/connectors/BinanceClient.cpp

Flow:
- Binance WebSocket BTCUSDT/ETHUSDT trade stream
- C++ manual JSON extraction
- TradePacket normalization
- lock-free SPSC queue
- FastOrderBook processing
- one-second terminal metrics

### 3. Kafka demo
Status: producer and consumer rewritten cleanly. Target enabled when protobuf and librdkafka are installed.

Files updated:
- cpp_engine/src/producer.cpp
- cpp_engine/src/consumer.cpp

Flow:
- kafka_producer creates protobuf Trade messages
- Kafka topic raw-trades
- kafka_consumer decodes protobuf and prints summary every 100k messages

## Important reality check

Use this claim:

The project has a real-market-data mode connected to Binance WebSocket and a synthetic C++ benchmark mode for validating 1M msg/sec. Kafka is kept separate from the lowest-latency hot path.

Do not claim Binance live stream provides 1M msg/sec.
