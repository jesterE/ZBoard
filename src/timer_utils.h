#pragma once

#include <assert.h>

// calculates the timer value correspoinding to a certain number of milliseconds passing, at a certain prescale setting.
// this is only correct if the master CPU clock is not being prescaled.
// it is recommended not to use this macro directly, as it performs no input validation, 
// and also returns a double (so should be used with care to avoid wasting cpu time)
#define TIMER_VALUE_MS__(ms, prescale) ((((double)ms / 1000.0) / (1.0 / ((double)F_CPU / (double)prescale))))

// the above macro is a bit complicated, a few small testcases are given below to make sure 
// all the parentheses are in the right spot
static_assert((uint8_t)TIMER_VALUE_MS__(5, 1024) == 78, "incorrect timer value");
static_assert((uint8_t)TIMER_VALUE_MS__(0.01, 1) == 160, "incorrect timer value");
static_assert((uint8_t)TIMER_VALUE_MS__(0.01, 8) == 20, "incorrect timer value");

/**
    \brief Returns the nearest value of timer0 corresponding to a certain number of milliseconds passing.

    This calculation is only correct if the master CPU clock is not prescaled.

    \param ms the number of milliseconds to compute the timer value for.
    Accepts a double as input, but note that the output value will be truncated according
    to the resolution of the timer.

    \param prescale The prescale setting of the timer.

    \return a uint8_t
*/
#define TIMER0_VALUE_MS(ms, prescale) ({ \
    static_assert(prescale == 1 || prescale == 8 || prescale == 64 || prescale == 1024, \
            "incorrect timer0 prescale"); \
    static_assert(TIMER_VALUE_MS__(ms, prescale) <= 255 && TIMER_VALUE_MS__(ms, prescale) >= 0, \
            "timer0 value is out of range"); \
    (uint8_t)TIMER_VALUE_MS__(ms, TIMER0_PRESCALE); \
})

