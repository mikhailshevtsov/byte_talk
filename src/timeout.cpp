#include <byte_talk/timeout.hpp>

namespace bt
{

deadline_t now() { return std::chrono::steady_clock::now(); }

}