#define ContactSlots_ClearTransient sub_0822B640
#define gContactSlotContext         gUnk_030025FC
#include "global.h"
struct ContactSlot {
    u16 tag, unk2, flags, value;
    u8 unk8[8];
};
struct ContactSlotContext {
    u8 unk0[0x20];
    struct ContactSlot slots[8];
};
extern struct ContactSlotContext *gUnk_030025FC;
void sub_0822B640(void)
{
    struct ContactSlotContext *ctx = gUnk_030025FC;
    if (ctx) {
        u16 mask = 4;
        u16 clearMask = 0xFFFB;
        struct ContactSlot *slot = ctx->slots;
        u16 invalid = 0xFFFF;
        s32 i = 7;
        do {
            u16 flags = slot->flags;
            u16 flag = mask & flags;
            if (flag)
                slot->flags = clearMask & flags;
            else {
                slot->tag |= invalid;
                slot->flags = flag;
            }
            slot++;
        } while (--i >= 0);
    }
}
