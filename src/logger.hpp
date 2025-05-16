#ifndef BYTETALK_LOGGER_HPP
#define BYTETALK_LOGGER_HPP

#include "net/errors.hpp"

#include <iostream>
#include <chrono>

namespace bt
{

class logger
{
public:
    logger(std::ostream& out) noexcept;

    void log(const net::error& e) noexcept;
    void log(const std::exception& e) noexcept;

    static const std::chrono::zoned_time<std::chrono::milliseconds> current_date_time() noexcept;

private:
    std::ostream& m_out;
};

}

#endif //BYTETALK_LOGGER_HPP