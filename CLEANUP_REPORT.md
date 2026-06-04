# Cleanup Report

This cleaned project keeps only files that currently contain useful code/config/docs and removes Git internals, empty placeholders, and duplicate empty config files.

## Removed
- `consumer/Dockerfile` — removed empty/unnecessary file
- `consumer/requirements.txt` — removed empty/unnecessary file
- `monitoring/prometheus.yml` — removed empty/unnecessary file
- `producer/Dockerfile` — removed empty/unnecessary file
- `producer/requirements.txt` — removed empty/unnecessary file
- `scripts/benchmark.sh` — removed empty/unnecessary file
- `scripts/create-topic.sh` — removed empty/unnecessary file

## Kept
- `.gitignore`
- `CMakeLists.txt`
- `docker-compose.yml`
- `market_data_pb2.py`
- `prometheus.yml`
- `README.md`
- `CMakeLists.benchmark.txt`
- `consumer/consumer.py`
- `cpp_engine/concurrentqueue.h`
- `cpp_engine/src/consumer.cpp`
- `cpp_engine/src/engine.cpp`
- `cpp_engine/src/engine_shared.hpp`
- `cpp_engine/src/producer.cpp`
- `cpp_engine/src/connectors/BinanceClient.cpp`
- `cpp_engine/src/connectors/BinanceClient.h`
- `cpp_engine/benchmarks/benchmark_engine.cpp`
- `docs/architecture_diagram.png`
- `docs/grafana_dashboard_throughput.png`
- `docs/prometheus_metrics_output.png`
- `producer/producer.py`
- `proto/market_data.proto`
- `tests/load_test.py`

## Important notes

- Root `prometheus.yml` was kept because Docker Compose mounts it.
- Empty `monitoring/prometheus.yml` was removed because it duplicates the root config name but has no content.
- Empty Dockerfiles and requirements files were removed because they contain no setup instructions.
- `.git/` was removed because it is repository metadata, not source code.
- `CMakeLists.benchmark.txt` was kept for the Phase 1 engine-only benchmark.
- `CMakeLists.txt` was kept for the full C++/Protobuf/WebSocket build, but it still needs dependencies installed.
