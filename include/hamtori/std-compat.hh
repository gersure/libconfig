#pragma once

#ifdef USE_STD_OPTIONAL_VARIANT_STRINGVIEW
#include <optional>
#include <string_view>
#include <variant>
#else
#include <experimental/optional>
#include <experimental/string_view>
#include <boost/variant.hpp>
#endif

namespace hamtori {

/// \cond internal

namespace compat {

#ifdef hamtori_USE_STD_OPTIONAL_VARIANT_STRINGVIEW

template <typename T>
using optional = std::optional<T>;

using nullopt_t = std::nullopt_t;

inline constexpr auto nullopt = std::nullopt;

template <typename T>
inline constexpr optional<std::decay_t<T>> make_optional(T&& value) {
    return std::make_optional(std::forward<T>(value));
}

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::basic_string_view<CharT, Traits>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
std::string string_view_to_string(const basic_string_view<CharT, Traits>& v) {
    return std::string(v);
}

template <typename... Types>
using variant = std::variant<Types...>;

template <std::size_t I, typename... Types>
constexpr std::variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v) {
    return std::get<I>(v);
}

template <std::size_t I, typename... Types>
constexpr const std::variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v) {
    return std::get<I>(v);
}

template <std::size_t I, typename... Types>
constexpr std::variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v) {
    return std::get<I>(v);
}

template <std::size_t I, typename... Types>
constexpr const std::variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v) {
    return std::get<I>(v);
}

template <typename U, typename... Types>
constexpr U& get(variant<Types...>& v) {
    return std::get<U>(v);
}

template <typename U, typename... Types>
constexpr const U& get(const variant<Types...>& v) {
    return std::get<U>(v);
}

template <typename U, typename... Types>
constexpr U&& get(variant<Types...>&& v) {
    return std::get<U>(v);
}

template <typename U, typename... Types>
constexpr const U&& get(const variant<Types...>&& v) {
    return std::get<U>(v);
}

template <typename U, typename... Types>
constexpr U* get_if(variant<Types...>* v) {
    return std::get_if<U>(v);
}

template <typename U, typename... Types>
constexpr const U* get_if(const variant<Types...>* v) {
    return std::get_if<U>(v);
}

#else

template <typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;

constexpr auto nullopt = std::experimental::nullopt;

template <typename T>
inline constexpr optional<std::decay_t<T>> make_optional(T&& value) {
    return std::experimental::make_optional(std::forward<T>(value));
}

template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::experimental::basic_string_view<CharT, Traits>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
std::string string_view_to_string(const basic_string_view<CharT, Traits>& v) {
    return v.to_string();
}

template <typename... Types>
using variant = boost::variant<Types...>;

template<typename U, typename... Types>
U& get(variant<Types...>& v) {
    return boost::get<U, Types...>(v);
}

template<typename U, typename... Types>
U&& get(variant<Types...>&& v) {
    return boost::get<U, Types...>(v);
}

template<typename U, typename... Types>
const U& get(const variant<Types...>& v) {
    return boost::get<U, Types...>(v);
}

template<typename U, typename... Types>
const U&& get(const variant<Types...>&& v) {
    return boost::get<U, Types...>(v);
}

template<typename U, typename... Types>
U* get_if(variant<Types...>* v) {
    return boost::get<U, Types...>(v);
}

template<typename U, typename... Types>
const U* get_if(const variant<Types...>* v) {
    return boost::get<U, Types...>(v);
}

#endif

using string_view = basic_string_view<char>;

} // namespace compat

/// \endcond

} // namespace hamtori
