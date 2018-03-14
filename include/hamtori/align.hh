#ifndef ALIGN_HH_
#define ALIGN_HH_

#include <cstdint>
#include <cstdlib>

namespace hamtori {

template <typename T>
inline constexpr
T align_up(T v, T align) {
    return (v + align - 1) & ~(align - 1);
}

template <typename T>
inline constexpr
T* align_up(T* v, size_t align) {
    static_assert(sizeof(T) == 1, "align byte pointers only");
    return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(v), align));
}

template <typename T>
inline constexpr
T align_down(T v, T align) {
    return v & ~(align - 1);
}

template <typename T>
inline constexpr
T* align_down(T* v, size_t align) {
    static_assert(sizeof(T) == 1, "align byte pointers only");
    return reinterpret_cast<T*>(align_down(reinterpret_cast<uintptr_t>(v), align));
}

}

#endif /* ALIGN_HH_ */
