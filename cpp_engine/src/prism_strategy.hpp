#pragma once

#include <cstdint>
#include <deque>
#include <string>

struct Bar {
    uint64_t ts = 0;
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    double volume = 0.0;
    double atr_14 = 0.0;
};

class PrismStrategy {
public:
    enum class State {
        IDLE,
        BREAKOUT,
        RETEST
    };

    struct Signal {
        bool valid = false;
        double entry_price = 0.0;
        double stop_loss = 0.0;
        double target1 = 0.0;
        double target2 = 0.0;
        double setup_score = 0.0;
        std::string reason;
    };

    PrismStrategy();

    void on_new_bar(const Bar& bar);

    bool has_signal() const;
    Signal current_signal() const;
    uint64_t bars_processed() const;

private:
    void detect_breakout(const Bar& bar);
    void detect_retest(const Bar& bar);
    double calculate_score(const Bar& bar);
    double average_volume(size_t lookback) const;

private:
    std::deque<Bar> history_;
    State state_;
    Signal signal_;

    uint64_t bars_processed_;
    uint64_t breakout_bar_index_;
    uint64_t last_signal_bar_;

    double breakout_level_;
};