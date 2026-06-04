#pragma once
#include "engine_shared.hpp"
#include <atomic>

class FastOrderBook {
private:
    double last_price_ = 0.0;
    double total_volume_ = 0.0;
    uint64_t updates_ = 0;

public:
    inline void match(const TradePacket& p) noexcept {
        // Phase-1 production-safe baseline: deterministic, allocation-free market-data update.
        // Real bid/ask FIFO matching can be added later on top of this interface.
        last_price_ = p.price;
        total_volume_ += p.volume;
        ++updates_;
    }

    uint64_t updates() const noexcept { return updates_; }
    double last_price() const noexcept { return last_price_; }
    double total_volume() const noexcept { return total_volume_; }
};
