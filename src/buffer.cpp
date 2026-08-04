#include <byte_talk/buffer.hpp>

#include <algorithm>


namespace bt
{

const char* buffer::first() const { return m_data.data(); }
char* buffer::first() { return m_data.data(); }

char* buffer::at(std::size_t index) { return first() + index; }
const char* buffer::at(std::size_t index) const { return first() + index; }

char* buffer::last() { return at(size()); }
const char* buffer::last() const { return at(size()); }

std::size_t buffer::bytes_left() const { return capacity() - size(); }

std::size_t buffer::size() const { return m_size; }
std::size_t buffer::capacity() const { return m_data.size(); }

void buffer::add_bytes(std::size_t n) { m_size += n; }
void buffer::extend(std::size_t n) { m_data.resize(capacity() + n); }

void buffer::clear() { m_size = 0; }

void buffer::push(std::string_view data)
{
    m_data.resize(m_size + data.size());
    std::copy(std::cbegin(data), std::cend(data), last());
    m_size += data.size();
}

void buffer::pop(std::size_t count)
{
    if (count < size())
    {
        std::copy(at(count), last(), first());
        m_size -= count;
    }
    else
        clear();
}

buffer::operator std::string_view() const { return std::string_view(first(), size()); }

bool buffer::empty() const { return m_size == 0; }

bool buffer::full() const { return size() >= capacity(); }

}
