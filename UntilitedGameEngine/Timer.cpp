#include "Timer.h"
using namespace std::chrono;

Timer::Timer()
{
    start = steady_clock::now();
    last = start;
}

float Timer::Mark()
{
    const auto old = last;
    last = steady_clock::now();
    const duration<float> frameTime = last - old;
    return frameTime.count();
}

float Timer::Peek() const
{
    return duration<float>(steady_clock::now() - start).count(); // kokonaisaika
}
