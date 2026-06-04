# PRISM Strategy Integration

This project now has four modes:

1. `benchmark_engine` — C++ engine-only latency/throughput benchmark.
2. `live_binance_engine` — real Binance WebSocket trade feed into the lock-free queue.
3. `kafka_producer` / `kafka_consumer` — Protobuf binary trade stream through Kafka.
4. `prism_backtest` — case-study strategy compiler/backtester.

## PRISM mode

The PRISM mode implements the Breakout Retest Acceptance strategy:

- previous 20-bar high breakout
- retest zone = breakout level ± 0.10 ATR
- close above retest midpoint
- close above VWAP
- close position >= 0.60
- score >= 6.5
- stop loss = min(entry − 1.2 ATR, retest zone low)
- target 1 = entry + 1.5R
- target 2 = entry + 2.5R
- invalidation, re-entry, and TTL logic

## Run

```cmd
cd C:\Users\Admin\realtime-financial-pipeline\build
Release\prism_backtest.exe ..\data\sample_market_data.csv ..\outputs\prism
```

If the sample CSV does not exist, the executable generates a deterministic sample dataset.

## Outputs

Generated in `outputs/prism/`:

- `setup_validation_report.json`
- `setup_score_log.csv`
- `entry_intent_log.csv`
- `trade_log.csv`
- `backtest_summary.json`
- `audit_log.json`

