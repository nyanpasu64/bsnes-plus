#ifdef DSP_CPP

static constexpr size_t COUNTER5_MASK = (1 << 8) - 1;
static constexpr size_t COUNTER1_MASK = (1 << 11) - 1;
static constexpr size_t COUNTER3_MASK = (1 << 9) - 1;

enum Counter : uint8 {
  Counter5,
  Counter1,
  Counter3,
};

struct CounterConfig {
  Counter counter;
  uint8 shift;
};

const static CounterConfig COUNTER_CONFIGS[32] = {
  {Counter1, 255},  // never fires
                  {Counter1, 11}, {Counter3, 9},
  {Counter5, 8},  {Counter1, 10}, {Counter3, 8},
  {Counter5, 7},  {Counter1, 9},  {Counter3, 7},
  {Counter5, 6},  {Counter1, 8},  {Counter3, 6},
  {Counter5, 5},  {Counter1, 7},  {Counter3, 5},
  {Counter5, 4},  {Counter1, 6},  {Counter3, 4},
  {Counter5, 3},  {Counter1, 5},  {Counter3, 3},
  {Counter5, 2},  {Counter1, 4},  {Counter3, 2},
  {Counter5, 1},  {Counter1, 3},  {Counter3, 1},
  {Counter5, 0},  {Counter1, 2},  {Counter3, 0},
                  {Counter1, 1},
                  {Counter1, 0},
};

template<typename T>
inline void decrement_mask(T & value, size_t mask) {
  value = (value - 1) & mask;
}

inline void DSP::counter_tick() {
  if (state.phase5 == 0) {
    state.phase5 = 5;
    decrement_mask(state.counter5, COUNTER5_MASK);
  }
  state.phase5--;

  decrement_mask(state.counter1, COUNTER1_MASK);

  if (state.phase3 == 0) {
    state.phase3 = 3;
    decrement_mask(state.counter3, COUNTER3_MASK);
  }
  state.phase3--;
}

//return true if counter event should trigger

inline bool is_trailing_zero(size_t val, size_t period_bits) {
  return (val >> period_bits) << period_bits == val;
}

inline bool DSP::counter_poll(unsigned rate) {
  if(rate == 0) return false;

  CounterConfig cfg = COUNTER_CONFIGS[rate];
  switch(cfg.counter) {
  case Counter5:
    return state.phase5 == 0 && is_trailing_zero(state.counter5, cfg.shift);
  case Counter1:
    return is_trailing_zero(state.counter1, cfg.shift);
  case Counter3:
    return state.phase3 == 0 && is_trailing_zero(state.counter3, cfg.shift);
  default:
    assert(false);
    abort();
  }
}

#endif
