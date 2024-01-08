#pragma once

#include <functional>
#include <string_view>
#include <type_traits>
#include <unordered_map>

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

template <typename... Bases>
struct overload : Bases...
{
    using is_transparent = void;
    using Bases::operator()...;
};

struct char_pointer_hash
{
    auto operator()(const char* ptr) const noexcept { return std::hash<std::string_view>{}(ptr); }
};

using transparent_string_hash =
    overload<std::hash<std::string>, std::hash<std::string_view>, char_pointer_hash>;

template <typename T>
using string_map = std::unordered_map<std::string, T, transparent_string_hash, std::equal_to<>>;

template <typename T>
auto get_or(const auto& js, auto key, T default_value) -> T
{
    if (js.contains(key))
    {
        return js[key].template get<T>();
    }
    return default_value;
}

template <typename F>
auto apply(const auto& js, auto key, F&& operation) -> void
{
    if (js.contains(key))
    {
        operation(js[key]);
    }
}
