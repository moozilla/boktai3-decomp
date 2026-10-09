#include "global.h"
s32 sub_0816E6F0(void *);
void sub_0816EA24(void);
void sub_08163EB8(void *, void (*)(void), u32);
void sub_0816E9F8(u8 *s) {
    s32 n = sub_0816E6F0(s);
    if (n >= 0) {
        s[0x4861] = n;
        sub_08163EB8(s, sub_0816EA24, 1);
    }
}
