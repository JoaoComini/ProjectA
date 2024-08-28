#pragma once

#include <iterator>
#include <unordered_map>
#include <string>

namespace embed
{
    struct file_data {
        const char* begin;
        const char* end;
    };

    class file
    {
    public:
        using iterator = const char*;
        using const_iterator = iterator;
        iterator begin() const noexcept { return _begin; }
        iterator cbegin() const noexcept { return _begin; }
        iterator end() const noexcept { return _end; }
        iterator cend() const noexcept { return _end; }

        file() = default;
        file(iterator begin, iterator end) noexcept : _begin(begin), _end(end) {}

        std::size_t size() const { return static_cast<std::size_t>(std::distance(begin(), end())); }

    private:
        const char* _begin = nullptr;
        const char* _end = nullptr;
    };
}