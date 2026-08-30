#ifndef BYTETALK_NET_UNIQUERESOURCE_HPP
#define BYTETALK_NET_UNIQUERESOURCE_HPP

#include <utility>

namespace bt::net
{

template <typename T, void(*Destructor)(T), T DefaultValue = T{}>
class unique_resource
{
public:
    using resource_type = T;
    static constexpr void(*destructor)(T) = Destructor;

    unique_resource() noexcept;
    ~unique_resource() noexcept;
    explicit unique_resource(T res) noexcept;

    unique_resource(unique_resource&& other) noexcept;
    unique_resource& operator=(unique_resource&& other) noexcept;

    unique_resource(const unique_resource& other) = delete;
    unique_resource& operator=(const unique_resource& other) = delete;

    void swap(unique_resource& other) noexcept;
    void reset() noexcept;
    T release() noexcept;
    void close() noexcept;

public:
    T get() const noexcept;
    bool valid() const noexcept;

    explicit operator bool() const noexcept;
    bool operator==(const unique_resource& other) const noexcept = default;

private:
    T m_res;
};

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>::unique_resource() noexcept
    : m_res{DefaultValue}
{}

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>::~unique_resource() noexcept
{
    close();
}

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>::unique_resource(T res) noexcept
    : m_res{res}
{}

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>::unique_resource(unique_resource&& other) noexcept
    : m_res{other.release()}
{}

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>& unique_resource<T, Destructor, DefaultValue>::operator=(unique_resource&& other) noexcept
{
    unique_resource temp(std::move(other));
    swap(temp);
    return *this;
}
template <typename T, void(*Destructor)(T), T DefaultValue>
void unique_resource<T, Destructor, DefaultValue>::swap(unique_resource& other) noexcept
{
    std::swap(m_res, other.m_res);
}

template <typename T, void(*Destructor)(T), T DefaultValue>
void unique_resource<T, Destructor, DefaultValue>::reset() noexcept
{
    m_res = DefaultValue;
}

template <typename T, void(*Destructor)(T), T DefaultValue>
T unique_resource<T, Destructor, DefaultValue>::release() noexcept
{
    return std::exchange(m_res, DefaultValue);
}
template <typename T, void(*Destructor)(T), T DefaultValue>
void unique_resource<T, Destructor, DefaultValue>::close() noexcept
{
    if (!valid())
        return;
    Destructor(m_res);
    reset();
}

template <typename T, void(*Destructor)(T), T DefaultValue>
T unique_resource<T, Destructor, DefaultValue>::get() const noexcept
{
    return m_res;
}

template <typename T, void(*Destructor)(T), T DefaultValue>
bool unique_resource<T, Destructor, DefaultValue>::valid() const noexcept
{
    return m_res != DefaultValue;
}

template <typename T, void(*Destructor)(T), T DefaultValue>
unique_resource<T, Destructor, DefaultValue>::operator bool() const noexcept
{
    return valid();
}
}

#endif //BYTETALK_NET_UNIQUERESOURCE_HPP