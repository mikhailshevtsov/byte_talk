#include "timer.hpp"

#include <sys/timerfd.h>
#include <unistd.h>

namespace bt::net
{

int timer::set_interval(int interval)
{
    itimerspec timerspec{};
    timerspec.it_value.tv_sec = interval;
    timerspec.it_interval.tv_sec = interval;
    return timerfd_settime(get(), 0, &timerspec, nullptr);
}

ssize_t timer::read(uint64_t& expirations)
{
    return ::read(get(), &expirations, sizeof(expirations));
}

timer make_timer()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    return timer{timerfd};
}

timer make_timer(int interval)
{
    timer _timer = make_timer();
    if (_timer)
        _timer.set_interval(interval);
    return _timer;
}

}