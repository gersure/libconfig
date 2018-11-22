#include "print_safe.hh"
#include "backtrace.hh"


namespace hamtori{

// Accumulates an in-memory backtrace and flush to stderr eventually.
// Async-signal safe.
class backtrace_buffer {
    static constexpr unsigned _max_size = 8 << 10;
    unsigned _pos = 0;
    char _buf[_max_size];
public:
    void flush() noexcept {
        print_safe(_buf, _pos);
        _pos = 0;
    }

    void append(const char* str, size_t len) noexcept {
        if (_pos + len >= _max_size) {
            flush();
        }
        memcpy(_buf + _pos, str, len);
        _pos += len;
    }

    void append(const char* str) noexcept { append(str, strlen(str)); }

    template <typename Integral>
    void append_decimal(Integral n) noexcept {
        char buf[sizeof(n) * 3];
        auto len = convert_decimal_safe(buf, sizeof(buf), n);
        append(buf, len);
    }

    template <typename Integral>
    void append_hex(Integral ptr) noexcept {
        char buf[sizeof(ptr) * 2];
        convert_zero_padded_hex_safe(buf, sizeof(buf), ptr);
        append(buf, sizeof(buf));
    }

    void append_backtrace() noexcept {
        backtrace([this] (frame f) {
            append("  ");
            if (!f.so->name.empty()) {
                append(f.so->name.c_str(), f.so->name.size());
                append("+");
            }

            append("0x");
            append_hex(f.addr);
            append("\n");
        });
    }
};

static void print_with_backtrace(backtrace_buffer& buf) noexcept {
    buf.append(".\nBacktrace:\n");
    buf.append_backtrace();
    buf.flush();
}

static void print_with_backtrace(const char* cause) noexcept {
    backtrace_buffer buf;
    buf.append(cause);
    print_with_backtrace(buf);
}

}
