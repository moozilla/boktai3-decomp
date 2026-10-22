#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.0%
// Open: the original stores a fresh `movs r1, #0` into a28[f10]. agbcc expands
// the byte store `s->f4 = 0x18` as (old & 0) | 0x18 and CSE reuses that zero
// pseudo (held in r4 across the calls), which shifts s/a28 to r5/r7. Writing
// the byte stores through `*&s->f4` / `*&s->f24` removes those zeros, but then
// CSE follows the jump path and reuses the IME-store zero instead (71.8%).
struct S {
    u8 pad0[2];
    volatile u8 f2;
    u8 pad3;
    u8 f4;
    u8 f5;
    u8 pad6[0xa];
    u8 f10;
    u8 pad11[3];
    u16 f14;
    u8 pad16[0xe];
    u8 f24;
    u8 pad25[3];
    u16 a28[4];
};
struct L {
    u8 pad[2];
    u8 f2;
    u8 f3;
};
extern struct S gUnk_03005390;
extern u16 *gUnk_03006A70[];
extern struct L *gUnk_03006A80;
void rfu_clearSlot(u32, u32);
void RfuManager_NotifyEvent(u32, u32);
void RfuManager_RequestMscCycleCompletion(void);
void RfuManager_DisconnectAndWait(u32);
// confidence=low: counts down the slot timeout and tears the link down when it expires.
void sub_082247CC(void)
{
    u16 ime = *(vu16 *)0x04000208;
    struct S *s;
    *(vu16 *)0x04000208 = 0;
    s = &gUnk_03005390;
    if (s->f4 == 0xf) {
        u16 *a;
        u16 *p;
        u32 i = s->f10;
        a = s->a28;
        p = &a[i];
        (*p)--;
        if (*p == 0 || *gUnk_03006A70[s->f10] == 0x27) {
            RfuManager_RequestMscCycleCompletion();
            s->f4 = 0x18;
            rfu_clearSlot(4, s->f10);
            s->f24 &= ~(1 << s->f10);
            a[s->f10] = 0;
        }
    }
    *(vu16 *)0x04000208 = ime;
    s = &gUnk_03005390;
    if (s->f4 == 0x18) {
        u32 z;
        if (s->f2 == 1)
            RfuManager_RequestMscCycleCompletion();
        z = s->f2;
        if (z == 0) {
            s->f5 = z;
            s->f4 = z;
            RfuManager_DisconnectAndWait(gUnk_03006A80->f2 | gUnk_03006A80->f3);
            s->f14 = z;
            RfuManager_NotifyEvent(0x25, 1);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082247CC);
#endif
