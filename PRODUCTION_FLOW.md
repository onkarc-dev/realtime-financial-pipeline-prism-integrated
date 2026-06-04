# Complete Project Flow

## Mode 1: benchmark_engine

Input:
- Synthetic C++ TradePacket messages.

Flow:
- Synthetic generator -> SPSC lock-free ring buffer -> FastOrderBook -> latency histogram.

Output:
- throughput_msg_per_sec
- latency_p50_ns
- latency_p90_ns
- latency_p99_ns
- latency_p999_ns
- latency_p99_ms

Purpose:
- Prove engine-only throughput and latency.
- Use this for the 1M msg/sec claim.

## Mode 2: live_binance_engine

Input:
- Real Binance Spot raw trade stream.
- Endpoint pattern: wss://stream.binance.com:9443/ws/<symbol>@trade

Flow:
- Binance WebSocket -> C++ BinanceClient -> manual JSON field extraction -> TradePacket -> SPSC queue -> FastOrderBook -> 1-second metrics.

Output:
- live_rate_msg_s
- total_processed
- ws_received
- ws_dropped
- last_price
- max_engine_latency_ns

Purpose:
- Real market data live demo.
- Do not claim Binance sends 1M msg/sec. The Binance trade stream is real-time but volume depends on actual exchange activity.

## Mode 3: kafka_producer / kafka_consumer

Input:
- Synthetic C++ protobuf encoded trades.

Flow:
- kafka_producer -> Kafka topic raw-trades -> kafka_consumer -> protobuf decode -> summary counters.

Output:
- producer sent_total
- consumer received_total

Purpose:
- Demonstrates Kafka/protobuf streaming and durable event distribution.
- Not the sub-0.50 ms hot path.
