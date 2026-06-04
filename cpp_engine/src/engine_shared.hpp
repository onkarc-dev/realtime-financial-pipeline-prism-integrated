#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

#ifdef _WIN32
  #include <intrin.h>
  #define PAUSE() _mm_pause()
#else
  #if defined(__x86_64__) || defined(__i386__)
    #define PAUSE() __builtin_ia32_pause()
  #else
    #define PAUSE() asm volatile("yield" ::: "memory")
  #endif
#endif

struct TradePacket {
    double price = 0.0;
    double volume = 0.0;
    char symbol[16]{};
    uint64_t exchange_ts_ns = 0;   // timestamp from exchange payload, if available
    uint64_t ingest_ts_ns = 0;     // local timestamp when feed handler received the message
    uint64_t trade_id = 0;
};

template<typename T, std::size_t Capacity>
class SimpleSPSCQueue {
private:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of two");
    static constexpr std::size_t Mask = Capacity - 1;

    alignas(64) T buffer_[Capacity];
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};

public:
    bool push(const T& item) noexcept {
        const std::size_t t = tail_.load(std::memory_order_relaxed);
        const std::size_t next = (t + 1) & Mask;
        if (next == head_.load(std::memory_order_acquire)) return false;
        buffer_[t] = item;
        tail_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& item) noexcept {
        const std::size_t h = head_.load(std::memory_order_relaxed);
        if (h == tail_.load(std::memory_order_acquire)) return false;
        item = buffer_[h];
        head_.store((h + 1) & Mask, std::memory_order_release);
        return true;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }
};

using TradeQueue = SimpleSPSCQueue<TradePacket, 1 << 20>;

extern TradeQueue trade_queue;
extern std::atomic<bool> running;
