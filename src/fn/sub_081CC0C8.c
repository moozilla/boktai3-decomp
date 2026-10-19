#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.6%
// foundry draft (worker best 95.6%); what differs is mostly register allocation and instruction order
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
s32 sub_081C8FB0(void *, void *, s32);

void sub_081CC0C8(void *a, void *p, u32 unused2)
{
    u32 r;
    if (B(9) != 0) {
        B(9) = 0;
        B(7) = 0;
        switch (B(1)) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                r = 0;
                break;
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xb;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    {
        u32 m = 0x1000;
        H(0xE) = m | H(0xE);
    }
    {
        void *callback = *(void **)((u8 *)p + 0x2E4);
        u8 result = Callback_InvokeActor(p, callback);
        if (result != 1) {
            if (sub_081C8FB0(a, p, 0) != 0) {
                u16 m2 = 0x800;
                H(0xE) = m2 | H(0xE);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081CC0C8);
#endif
