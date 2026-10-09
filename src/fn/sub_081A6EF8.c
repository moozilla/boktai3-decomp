#include "global.h"

void sub_0821FF8C(void *, void *, void *);
void sub_08159860(void *, void *, s32);

void sub_081A6EF8(void *a, u8 *b, u8 *c) {
    u16 value;
    u32 word;
    u16 tail;
    sub_0821FF8C(a, b, c);
    value = *(u16 *)(b + 0x3E);
    *(u16 *)(c + 0x128) = value;
    word = *(u32 *)((u8 *)a + 0x38);
    *(u32 *)(c + 0x124) = word;
    tail = *(u16 *)((u8 *)a + 0x44);
    *(u16 *)(c + 0x12E) = tail;
    sub_08159860(a, b, 10);
}
