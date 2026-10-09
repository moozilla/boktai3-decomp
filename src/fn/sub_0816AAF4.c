#include "global.h"
void sub_08220F70(void *, void *);
void sub_08176B00(void);
void sub_08163EA0(void *, void (*)(void));
void sub_0816AAF4(u8 *s) {
    sub_08220F70(s + 0x3BE0, s + 0xC4);
    (*(u16 *)(s + 0xA4C))++;
    if (*(u16 *)(s + 0xA4C) > 0x95)
        sub_08163EA0(s, sub_08176B00);
}
