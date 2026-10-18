#define DirectionalActor_UpdateVisibilityDelay sub_08186B34
#include "global.h"
struct DirectionalActorDisplay { u32 unknown0[2], flags; };
struct DirectionalActor {
    u8 unknown0[0xB4];
    struct DirectionalActorDisplay display;
    u8 unknownC0[0xA1C];
    u8 trigger[0x56];
    u16 flagsB32;
    u8 unknownB34[9];
    u8 pendingVisibility, delay;
};
u32 Trigger_ConsumePending(void *, u32);
void sub_08186B34(struct DirectionalActor *p)
{
    s32 mode = p->pendingVisibility;
    switch (mode) {
    case 0:
    case 1:
        if (p->delay) {
            p->delay--;
        } else {
            if (mode != 0)
                ((struct DirectionalActorDisplay *)((u8 *)p + 0xB4))->flags |= 1;
            else
                ((struct DirectionalActorDisplay *)((u8 *)p + 0xB4))->flags &= ~1;
            p->flagsB32 &= 0xfbff;
            p->pendingVisibility = 0xff;
            Trigger_ConsumePending(p->trigger, 1);
        }
        break;
    }
}
