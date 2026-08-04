#ifndef BYTETALK_BUFFER_HPP
#define BYTETALK_BUFFER_HPP

#include <vector>
#include <string_view>


namespace bt
{

class buffer
{
public:
    const char* first() const;
    char* first();

    char* last();
    const char* last() const;

    char* at(std::size_t index);
    const char* at(std::size_t index) const;

    std::size_t bytes_left() const;

    std::size_t size() const;
    std::size_t capacity() const;

    void add_bytes(std::size_t size);
    void extend(std::size_t size);

    void clear();

    void push(std::string_view data);
    void pop(std::size_t count);

    operator std::string_view() const;

    bool empty() const;
    bool full() const;

private:
    static constexpr std::size_t CAPACITY = 1024;

private:
    std::vector<char> m_data = std::vector<char>(CAPACITY);
    std::size_t m_size = 0;
};

}

#endif //BYTETALK_BUFFER_HPP