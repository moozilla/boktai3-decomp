#include "global.h"
struct Slot {
    s16 a, b, c, pad, d, e;
    u16 flags, value;
};
struct Context {
    u8 pad[0x18];
    struct Slot slots[8];
};
extern struct Context *gUnk_030025FC;
u8 SceneStimulus_TestSquareOverlap(s32, s32, s32, s32, s32, s32);
s8 sub_0822B7E4(s32 a, s32 b, s32 c, u8 mask, u16 value, s32 start)
{
    struct Context *ctx = gUnk_030025FC;
    struct Slot *slot;
    s32 index;
    if (ctx != NULL)
        goto begin;
    goto fail;
found:
    return index;
begin:
    index = start;
    if (index <= 7) {
        slot = (struct Slot *)(index * 16 + (u32)ctx->slots);
        do {
            if (!(slot->flags & 4) && (!slot->value || slot->value != value)) {
                if (slot->flags & 0x80)
                    goto found;
                if (slot->d >= 0 && (slot->flags & mask) &&
                    SceneStimulus_TestSquareOverlap(slot->a, slot->c, slot->d, a, b, c))
                    goto found;
            }
            slot++;
            index++;
        } while (index <= 7);
    }
fail:
    return -1;
}
