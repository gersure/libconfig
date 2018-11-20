#pragma once

#include <fmt/ostream.h>
#include <fmt/printf.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include "sstring.hh"

#if 0
inline
std::ostream&
operator<<(std::ostream& os, const void* ptr) {
    auto flags = os.flags();
    os << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    os.flags(flags);
    return os;
}
#endif

inline
std::ostream&
operator<<(std::ostream&& os, const void* ptr) {
    return os << ptr; // selects non-rvalue version
}

namespace hamtori {

template <typename... A>
std::ostream&
fprint(std::ostream& os, const char* fmt, A&&... a) {
    ::fmt::fprintf(os, fmt, std::forward<A>(a)...);
    return os;
}

template <typename... A>
void
print(const char* fmt, A&&... a) {
    ::fmt::printf(fmt, std::forward<A>(a)...);
}

template <typename... A>
std::string
sprint(const char* fmt, A&&... a) {
    std::ostringstream os;
    ::fmt::fprintf(os, fmt, std::forward<A>(a)...);
    return os.str();
}

template <typename... A>
std::string
sprint(const sstring& fmt, A&&... a) {
    std::ostringstream os;
    ::fmt::fprintf(os, fmt.c_str(), std::forward<A>(a)...);
    return os.str();
}

template <typename Iterator>
std::string
format_separated(Iterator b, Iterator e, const char* sep = ", ") {
    std::string ret;
    if (b == e) {
        return ret;
    }
    ret += *b++;
    while (b != e) {
        ret += sep;
        ret += *b++;
    }
    return ret;
}

template <typename TimePoint>
struct usecfmt_wrapper {
    TimePoint val;
};

template <typename TimePoint>
inline
usecfmt_wrapper<TimePoint>
usecfmt(TimePoint tp) {
    return { tp };
};

template <typename Clock, typename Rep, typename Period>
std::ostream&
operator<<(std::ostream& os, usecfmt_wrapper<std::chrono::time_point<Clock, std::chrono::duration<Rep, Period>>> tp) {
    auto usec = std::chrono::duration_cast<std::chrono::microseconds>(tp.val.time_since_epoch()).count();
    std::ostream tmp(os.rdbuf());
    tmp << std::setw(12) << (usec / 1000000) << "." << std::setw(6) << std::setfill('0') << (usec % 1000000);
    return os;
}

template <typename... A>
void
log(A&&... a) {
    std::cout << usecfmt(std::chrono::high_resolution_clock::now()) << " ";
    print(std::forward<A>(a)...);
}

/**
 * Evaluate the formatted string in a native fmt library format
 *
 * @param fmt format string with the native fmt library syntax
 * @param a positional parameters
 *
 * @return sstring object with the result of applying the given positional
 *         parameters on a given format string.
 */
template <typename... A>
sstring
format(const char* fmt, A&&... a) {
    fmt::memory_buffer out;
    fmt::format_to(out, fmt, std::forward<A>(a)...);
    return sstring{out.data(), out.size()};
}

// temporary, use fmt::print() instead
template <typename... A>
std::ostream&
fmt_print(std::ostream& os, const char* format, A&&... a) {
    fmt::print(os, format, std::forward<A>(a)...);
    return os;
}

}
