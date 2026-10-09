#include "global.h"
void sub_08030BF8(void);
void sub_08033468(void);
void sub_08033590(void);
void sub_0816EA98(void);
void sub_08163EB8(void *, void (*)(void), u32);
void sub_0816E540(u8 *s, u32 value) {
    s[0x4861] = value;
    sub_08030BF8();
    sub_08033468();
    sub_08033590();
    sub_08163EB8(s, sub_0816EA98, 1);
}
