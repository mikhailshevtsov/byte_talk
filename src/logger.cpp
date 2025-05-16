#include "logger.hpp"

namespace bt
{

logger::logger(std::ostream& out) noexcept
    : m_out{out}
{}

void logger::log(const net::error& e) noexcept
{
    m_out << current_date_time() << " -> " << e.type() << " exception on socket " << e.sockfd() << " : " << e.what() << "\n";
}

void logger::log(const std::exception& e) noexcept
{
    m_out << current_date_time() << " -> unknown exception " << e.what() << "\n";
}

const std::chrono::zoned_time<std::chrono::milliseconds> logger::current_date_time() noexcept
{
    return {std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())};
}

}