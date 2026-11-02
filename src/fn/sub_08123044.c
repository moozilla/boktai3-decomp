#include "global.h"
struct SpriteNotice {
    u8 unk0[0x42];
    u8 state;
    u8 unk43[5];
};
struct SpriteNoticeManager {
    u8 task[0x18];
    void *animationData, *spriteData;
    struct SpriteNotice slots[32];
    u32 occupiedMask;
};
extern void *gUnk_08610728[];
s32 sub_08249240(void *, void *, void *);
static inline u8 Occupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
static inline u32 *SlotMask(struct SpriteNoticeManager *manager)
{
    return &manager->occupiedMask;
}
s32 SpriteNotice_TickManager(struct SpriteNoticeManager *manager)
{
    struct SpriteNotice *start;
    s32 i;
    u32 *mask;
    void **table;
    u8 *state;
    struct SpriteNotice *slot;
    if (!manager->occupiedMask)
        return 0;
    i = 0;
    start = manager->slots;
    mask = SlotMask(manager);
    table = gUnk_08610728;
    state = &manager->slots[0].state;
    slot = start;
    for (; i < 32;) {
        if (Occupied(mask, i))
            sub_08249240(manager, slot, table[*state]);
        state += sizeof(*slot);
        slot++;
        i++;
    }
    return 0;
}
