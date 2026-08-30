#ifndef BYTETALK_TIMEOUT_HPP
#define BYTETALK_TIMEOUT_HPP

#include <chrono>

namespace bt
{

using timeout_t = std::chrono::steady_clock::duration;
using deadline_t = std::chrono::steady_clock::time_point;

deadline_t now();

}

#endif //BYTETALK_TIMEOUT_HPP