#include "connectors/BinanceClient.h"
#include "engine_shared.hpp"
#include "order_book.hpp"
#include "prism_live_engine.hpp"
#include "time_utils.hpp"
#include "trade_manager.hpp"

#include <atomic>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

TradeQueue trade_queue;
std::atomic<bool> running{true};

namespace {

void signal_handler(int) {
    running.store(false, std::memory_order_relaxed);
}

} // namespace

int main(int argc, char** argv) {
    std::string symbol = "btcusdt";

    if (argc >= 2) {
        symbol = argv[1];
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    FastOrderBook book;

    constexpr int prism_bar_seconds = 10;
    PrismLiveEngine prism(prism_bar_seconds);

    TradeManager trade_manager("trade_log.csv");

    std::atomic<uint64_t> processed{0};
    std::atomic<uint64_t> max_engine_latency_ns{0};
    std::atomic<uint64_t> last_price_scaled{0};
    std::atomic<uint64_t> prism_signals{0};

    BinanceClient client(symbol);

    std::thread engine_thread([&]() {
        TradePacket p{};

        bool previous_signal_state = false;

        while (running.load(std::memory_order_relaxed)) {
            if (trade_queue.pop(p)) {
                const uint64_t start = now_ns();

                const uint64_t current_processed =
                    processed.load(std::memory_order_relaxed);

                // For live testing: update order book every 4th tick
                // to reduce hot-path load and keep msg/sec high.
                // OrderBook disabled during PRISM strategy testing.
	        // Enable later after strategy is stable.
	        // book.match(p);

                prism.on_trade(p);

                trade_manager.on_price(p.price);

                const bool current_signal_state =
                    prism.has_signal();

                if (current_signal_state && !previous_signal_state) {
                    const PrismSignal signal =
                        prism.last_signal();

                    prism_signals.fetch_add(
                        1,
                        std::memory_order_relaxed
                    );

                    trade_manager.on_signal(signal);
                }

                previous_signal_state =
                    current_signal_state;

                processed.fetch_add(
                    1,
                    std::memory_order_relaxed
                );

                last_price_scaled.store(
                    static_cast<uint64_t>(p.price * 100.0),
                    std::memory_order_relaxed
                );

                const uint64_t latency =
                    now_ns() - start;

                if (latency < 1000000000ULL) {
                    uint64_t prev =
                        max_engine_latency_ns.load(
                            std::memory_order_relaxed
                        );

                    while (
                        latency > prev &&
                        !max_engine_latency_ns.compare_exchange_weak(
                            prev,
                            latency
                        )
                    ) {
                        // retry
                    }
                }

            } else {
                PAUSE();
            }
        }
    });

    std::thread feed_thread([&]() {
        client.run();
    });

    std::cout
        << "live_binance_engine running. Symbol: "
        << symbol
        << "@trade\n";

    std::cout
        << "PRISM live engine enabled: "
        << prism_bar_seconds
        << "-second bars\n";

    std::cout
        << "TradeManager enabled: live entry/SL/targets + CSV + JSON\n";

    std::cout
        << "OrderBook throttled: processing every 4th tick\n";

    std::cout
        << "Press Ctrl+C to stop.\n";

    uint64_t last_processed = 0;
    uint64_t last_signals = 0;

    while (running.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        const uint64_t now_processed =
            processed.load(std::memory_order_relaxed);

        const uint64_t per_sec =
            (now_processed - last_processed) / 2;

        last_processed = now_processed;

        const uint64_t now_signals =
            prism_signals.load(std::memory_order_relaxed);

        const uint64_t signals_per_sec =
            (now_signals - last_signals) / 2;

        last_signals = now_signals;

        const double last_price =
            static_cast<double>(
                last_price_scaled.load(std::memory_order_relaxed)
            ) / 100.0;

        std::cout
            << "live_rate_msg_s=" << per_sec
            << " total_processed=" << now_processed
            << " ws_received=" << client.received()
            << " ws_dropped=" << client.dropped()
            << " last_price=" << last_price
            << " bars=" << prism.bars_count()
            << " prism_signals_total=" << now_signals
            << " prism_signals_s=" << signals_per_sec
            << " open_trade=" << (trade_manager.has_open_trade() ? 1 : 0)
            << " total_trades=" << trade_manager.total_trades()
            << " wins=" << trade_manager.wins()
            << " losses=" << trade_manager.losses()
            << " breakevens=" << trade_manager.breakevens()
            << " gross_R=" << trade_manager.gross_r()
            << " avg_R=" << trade_manager.average_r()
            << " max_engine_latency_ns="
            << max_engine_latency_ns.load(std::memory_order_relaxed)
            << "\n";
    }

    client.stop();

    if (feed_thread.joinable()) {
        feed_thread.join();
    }

    if (engine_thread.joinable()) {
        engine_thread.join();
    }

    trade_manager.write_summary_json("performance_summary.json");

    std::cout
        << "Stopped. total_processed="
        << processed.load(std::memory_order_relaxed)
        << " prism_signals="
        << prism_signals.load(std::memory_order_relaxed)
        << " total_trades="
        << trade_manager.total_trades()
        << " wins="
        << trade_manager.wins()
        << " losses="
        << trade_manager.losses()
        << " breakevens="
        << trade_manager.breakevens()
        << " gross_R="
        << trade_manager.gross_r()
        << " avg_R="
        << trade_manager.average_r()
        << "\n";

    std::cout
        << "Generated files: trade_log.csv, performance_summary.json\n";

    return 0;
}