#include "connectors/BinanceClient.h"
#include "engine_shared.hpp"
#include "order_book.hpp"
#include "time_utils.hpp"

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
}

int main(int argc, char** argv) {
    std::string symbol = "btcusdt";
    if (argc >= 2) symbol = argv[1];

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    FastOrderBook book;
    std::atomic<uint64_t> processed{0};
    std::atomic<uint64_t> max_engine_latency_ns{0};
    std::atomic<uint64_t> last_price_scaled{0};

    BinanceClient client(symbol);

    std::thread engine_thread([&]() {
        TradePacket p{};
        while (running.load(std::memory_order_relaxed)) {
            if (trade_queue.pop(p)) {
                const uint64_t start = now_ns();
                book.match(p);
                const uint64_t latency = now_ns() - start;
                processed.fetch_add(1, std::memory_order_relaxed);
                last_price_scaled.store(static_cast<uint64_t>(p.price * 100.0), std::memory_order_relaxed);
                uint64_t prev = max_engine_latency_ns.load(std::memory_order_relaxed);
                while (latency > prev && !max_engine_latency_ns.compare_exchange_weak(prev, latency)) {}
            } else {
                PAUSE();
            }
        }
    });

    std::thread feed_thread([&]() { client.run(); });

    std::cout << "live_binance_engine running. Symbol: " << symbol << "@trade\n";
    std::cout << "Press Ctrl+C to stop.\n";

    uint64_t last_processed = 0;
    while (running.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const uint64_t now_processed = processed.load(std::memory_order_relaxed);
        const uint64_t per_sec = now_processed - last_processed;
        last_processed = now_processed;
        const double last_price = static_cast<double>(last_price_scaled.load(std::memory_order_relaxed)) / 100.0;
        std::cout << "live_rate_msg_s=" << per_sec
                  << " total_processed=" << now_processed
                  << " ws_received=" << client.received()
                  << " ws_dropped=" << client.dropped()
                  << " last_price=" << last_price
                  << " max_engine_latency_ns=" << max_engine_latency_ns.load(std::memory_order_relaxed)
                  << "\n";
    }

    client.stop();
    if (feed_thread.joinable()) feed_thread.join();
    if (engine_thread.joinable()) engine_thread.join();

    std::cout << "Stopped. total_processed=" << processed.load() << "\n";
    return 0;
}
