#include "global.h"
extern void sub_08013440(void *);
extern void sub_08076034(void *, s32);
extern void sub_080A60E0(void);
static inline u8 take6030(u8 *s) { if (s[0x2B2]) { s[0x2B2] = 0; return 1; } return 0; }
struct Obj6030 { u32 flags; u8 pad[0x1C]; };
struct Part6030 { u8 pad[8]; u32 flags; };
void sub_080A6030(u8 *s, s32 arg, u32 unused) {
    u32 queued = take6030(s);
    if (queued) {
        u32 mask = 1;
        if (s[8] == 0) {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            u32 value = obj->flags;
            obj->flags = value | mask;
        } else {
            struct Obj6030 *obj = *(struct Obj6030 **)(s + 4);
            struct Part6030 *part = (struct Part6030 *)((u8 *)obj + 0x20);
            u32 value = part->flags;
            part->flags = value | mask;
        }
        sub_08013440(s + 0x320);
    } else if (arg > 0x1A) {
        void (*fn)(void);
        u32 code;
        u32 *flags;
        u32 mask;
        sub_08076034(s, 9);
        fn = sub_080A60E0;
        code = 0xD;
        s[0x2B2] = 1;
        s[0x2B0] = queued;
        s[0x2B5] = code;
        *(void (**)(void))(s + 0x21C) = fn;
        *(u16 *)(s + 0x2C8) = queued;
        flags = (u32 *)(s + 0x114);
        mask = ~1;
        *flags &= mask;
        *(u32 *)(s + 0x104) = queued;
    } else {
        s[0x2B3] = queued;
    }
}
