#include "prism_strategy.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

// ================================================================
// LIVE TEST SETTINGS
// ================================================================
// Testing mode:
// LOOKBACK_BARS = 10 gives faster breakout detection.
// After testing, change back to 20.
static constexpr size_t LOOKBACK_BARS = 10;

// Retest can happen within 30 bars.
// With 10-sec bars, this means 5 minutes.
static constexpr size_t MAX_RETEST_BARS = 30;

// Prevent too many signals close together.
static constexpr size_t SIGNAL_COOLDOWN_BARS = 5;

// Testing threshold.
// After confirming stable trades, change to 7.0.
static constexpr double MIN_SCORE = 5.5;

// Retest tolerance around breakout level.
static constexpr double RETEST_TOLERANCE_PCT = 0.001;

// Testing close-position threshold.
// After testing, change to 0.65.
static constexpr double MIN_CLOSE_POSITION = 0.50;

// Keep true while debugging.
// Later set false for lower console overhead.
static constexpr bool DEBUG_BAR_LOGS = true;
static constexpr bool DEBUG_RETEST_LOGS = true;

PrismStrategy::PrismStrategy()
    : state_(State::IDLE),
      bars_processed_(0),
      breakout_bar_index_(0),
      last_signal_bar_(0),
      breakout_level_(0.0)
{
}

void PrismStrategy::on_new_bar(const Bar& bar)
{
    ++bars_processed_;

    history_.push_back(bar);

    if (history_.size() > 200) {
        history_.pop_front();
    }

    signal_ = Signal{};

    if (DEBUG_BAR_LOGS) {
        std::cout
            << "[BAR]"
            << " index=" << bars_processed_
            << " close=" << bar.close
            << " high=" << bar.high
            << " low=" << bar.low
            << " volume=" << bar.volume
            << " atr14=" << bar.atr_14
            << " state=" << static_cast<int>(state_)
            << "\n";
    }

    if (history_.size() < LOOKBACK_BARS + 1) {
        if (DEBUG_BAR_LOGS) {
            std::cout
                << "[WAITING_HISTORY]"
                << " have=" << history_.size()
                << " need=" << (LOOKBACK_BARS + 1)
                << "\n";
        }

        return;
    }

    if (state_ == State::IDLE) {
        detect_breakout(bar);
        return;
    }

    if (state_ == State::BREAKOUT || state_ == State::RETEST) {
        detect_retest(bar);
        return;
    }
}

void PrismStrategy::detect_breakout(const Bar& bar)
{
    if (bars_processed_ - last_signal_bar_ < SIGNAL_COOLDOWN_BARS) {
        return;
    }

    double previous_high =
        history_[history_.size() - LOOKBACK_BARS - 1].high;

    for (
        size_t i = history_.size() - LOOKBACK_BARS;
        i < history_.size() - 1;
        ++i
    ) {
        previous_high =
            std::max(previous_high, history_[i].high);
    }

    if (DEBUG_BAR_LOGS) {
        std::cout
            << "[BREAKOUT_CHECK]"
            << " close=" << bar.close
            << " previous_high=" << previous_high
            << "\n";
    }

    if (bar.close > previous_high) {
        breakout_level_ = previous_high;
        breakout_bar_index_ = bars_processed_;
        state_ = State::BREAKOUT;

        std::cout
            << "[BREAKOUT_FOUND]"
            << " close=" << bar.close
            << " level=" << breakout_level_
            << " bar_index=" << breakout_bar_index_
            << "\n";
    }
}

void PrismStrategy::detect_retest(const Bar& bar)
{
    if (breakout_level_ <= 0.0) {
        state_ = State::IDLE;
        return;
    }

    const uint64_t bars_since_breakout =
        bars_processed_ - breakout_bar_index_;

    if (bars_since_breakout > MAX_RETEST_BARS) {
        state_ = State::IDLE;
        breakout_level_ = 0.0;
        breakout_bar_index_ = 0;

        std::cout
            << "[RETEST_EXPIRED]"
            << " bars_since_breakout=" << bars_since_breakout
            << "\n";

        return;
    }

    if (bars_processed_ - last_signal_bar_ < SIGNAL_COOLDOWN_BARS) {
        state_ = State::IDLE;
        breakout_level_ = 0.0;
        breakout_bar_index_ = 0;
        return;
    }

    const double range =
        bar.high - bar.low;

    if (range <= 0.0) {
        return;
    }

    const double tolerance =
        breakout_level_ * RETEST_TOLERANCE_PCT;

    const bool touched_breakout =
        bar.low <= breakout_level_ + tolerance;

    const double close_position =
        (bar.close - bar.low) / range;

    const double avg_vol =
        average_volume(LOOKBACK_BARS);

    // Temporarily relaxed for live testing.
    const bool volume_confirmed = true;

    const bool accepted =
        touched_breakout &&
        close_position >= MIN_CLOSE_POSITION &&
        bar.close >= breakout_level_ &&
        volume_confirmed;

    const double score =
        calculate_score(bar);

    if (DEBUG_RETEST_LOGS) {
        std::cout
            << "[RETEST_CHECK]"
            << " close=" << bar.close
            << " breakout=" << breakout_level_
            << " low=" << bar.low
            << " high=" << bar.high
            << " close_position=" << close_position
            << " volume=" << bar.volume
            << " avg_volume=" << avg_vol
            << " volume_ok=" << (volume_confirmed ? 1 : 0)
            << " touched=" << (touched_breakout ? 1 : 0)
            << " accepted=" << (accepted ? 1 : 0)
            << " score=" << score
            << "\n";
    }

    if (!accepted) {
        state_ = State::RETEST;
        return;
    }

    if (score < MIN_SCORE) {
        state_ = State::RETEST;
        return;
    }

    const double entry =
        bar.close;

    double atr_component = 0.0;

    if (bar.atr_14 > 0.0) {
        atr_component = bar.atr_14 * 0.75;
    } else {
        atr_component = range * 1.25;
    }

    const double structure_stop =
        bar.low - (range * 0.25);

    const double atr_stop =
        entry - atr_component;

    const double stop =
        std::min(structure_stop, atr_stop);

    const double risk =
        entry - stop;

    if (risk <= 0.0) {
        state_ = State::RETEST;
        return;
    }

    signal_.valid = true;
    signal_.entry_price = entry;
    signal_.stop_loss = stop;
    signal_.target1 = entry + risk * 1.5;
    signal_.target2 = entry + risk * 2.5;
    signal_.setup_score = score;
    signal_.reason = "PRISM_ATR_BREAKOUT_RETEST_TEST_MODE";

    last_signal_bar_ = bars_processed_;

    std::cout
        << "\n========== PRISM SIGNAL GENERATED ==========\n"
        << "Entry  : " << signal_.entry_price << "\n"
        << "Stop   : " << signal_.stop_loss << "\n"
        << "Target1: " << signal_.target1 << "\n"
        << "Target2: " << signal_.target2 << "\n"
        << "Score  : " << signal_.setup_score << "\n"
        << "Reason : " << signal_.reason << "\n"
        << "===========================================\n\n";

    state_ = State::IDLE;
    breakout_level_ = 0.0;
    breakout_bar_index_ = 0;
}

double PrismStrategy::calculate_score(const Bar& bar)
{
    double score = 0.0;

    const double range =
        bar.high - bar.low;

    if (range > 0.0) {
        const double close_position =
            (bar.close - bar.low) / range;

        if (close_position >= 0.50) {
            score += 3.0;
        }

        if (close_position >= 0.75) {
            score += 1.0;
        }
    }

    if (bar.close >= bar.open) {
        score += 2.0;
    }

    // Testing mode: volume scoring remains permissive.
    score += 1.5;

    score += 2.0;

    return std::min(score, 10.0);
}

double PrismStrategy::average_volume(size_t lookback) const
{
    if (history_.empty()) {
        return 0.0;
    }

    const size_t n =
        std::min(lookback, history_.size());

    double sum = 0.0;

    for (size_t i = history_.size() - n; i < history_.size(); ++i) {
        sum += history_[i].volume;
    }

    return sum / static_cast<double>(n);
}

bool PrismStrategy::has_signal() const
{
    return signal_.valid;
}

PrismStrategy::Signal PrismStrategy::current_signal() const
{
    return signal_;
}

uint64_t PrismStrategy::bars_processed() const
{
    return bars_processed_;
}