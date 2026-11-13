/* Runtime linked into every C snippet (see agent.h). Freestanding: no libc. */
#include <stdarg.h>
#include <stdint.h>

__attribute__((import_module("env"), import_name("agent_print")))
void agent_print(const char *text, unsigned length);
__attribute__((import_module("env"), import_name("port_icall")))
long long port_icall(void *function, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned,
                     unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);

unsigned agent_result;
static char buffer[4096];
static unsigned used;

static void flush(void)
{
    if (used) agent_print(buffer, used);
    used = 0;
}

static void put(char c)
{
    if (used == sizeof(buffer)) flush();
    buffer[used++] = c;
}

static void put_number(unsigned long long value, unsigned base, int upper, int negative,
                       int width, int zero, int left)
{
    char digits[24];
    int n = 0, total;
    const char *set = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    do {
        digits[n++] = set[value % base];
        value /= base;
    } while (value);
    total = n + negative;
    if (negative && zero) put('-');
    if (!left)
        while (total < width) { put(zero ? '0' : ' '); total++; }
    if (negative && !zero) put('-');
    while (n) put(digits[--n]);
    if (left)
        while (total < width) { put(' '); total++; }
}

void print(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    for (; *format; format++) {
        int width = 0, zero = 0, left = 0;
        if (*format != '%') { put(*format); continue; }
        format++;
        for (;; format++) {
            if (*format == '-') left = 1;
            else if (*format == '0') zero = 1;
            else break;
        }
        while (*format >= '0' && *format <= '9') width = width * 10 + (*format++ - '0');
        while (*format == 'l' || *format == 'h') format++;
        switch (*format) {
        case 'd': case 'i': {
            int v = va_arg(ap, int);
            put_number(v < 0 ? -(long long)v : v, 10, 0, v < 0, width, zero, left);
            break;
        }
        case 'u': put_number(va_arg(ap, unsigned), 10, 0, 0, width, zero, left); break;
        case 'x': put_number(va_arg(ap, unsigned), 16, 0, 0, width, zero, left); break;
        case 'X': put_number(va_arg(ap, unsigned), 16, 1, 0, width, zero, left); break;
        case 'o': put_number(va_arg(ap, unsigned), 8, 0, 0, width, zero, left); break;
        case 'p': put('0'); put('x'); put_number((uintptr_t)va_arg(ap, void *), 16, 0, 0, 8, 1, 0); break;
        case 'c': put((char)va_arg(ap, int)); break;
        case 's': {
            const char *s = va_arg(ap, const char *);
            int n = 0;
            if (!s) s = "(null)";
            while (s[n]) n++;
            if (!left) while (n < width) { put(' '); width--; }
            while (*s) put(*s++);
            if (left) while (n < width) { put(' '); width--; }
            break;
        }
        case '%': put('%'); break;
        case 0: format--; break;
        default: put('%'); put(*format);
        }
    }
    va_end(ap);
    flush();
}

void dump(const void *address, unsigned len)
{
    const unsigned char *p = address;
    unsigned i;
    for (i = 0; i < len; i += 16) {
        unsigned j;
        print("%08x ", (unsigned)(uintptr_t)(p + i));
        for (j = i; j < i + 16 && j < len; j++) print(" %02x", p[j]);
        print("\n");
    }
}

unsigned call_address(unsigned address, unsigned a0, unsigned a1, unsigned a2, unsigned a3)
{
    return (unsigned)port_icall((void *)(uintptr_t)address, a0, a1, a2, a3, 0, 0, 0, 0, 0, 0, 0, 0);
}
