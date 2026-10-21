// MATCHING_CONCESSION: register-pinning
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
s32 SpriteNotice_TickManager(struct SpriteNoticeManager *manager)
{
    struct SpriteNotice *start;
    s32 i;
    u32 *mask;
    register u32 off2 asm("r2");
    void **table;
    u8 *state;
    struct SpriteNotice *slot;
    register u32 off asm("r1") = 0x920;
    asm("" : "+r"(off));
    if (*(u32 *)((u8 *)manager + off)) {
        asm("" ::"r"(off));
        i = 0;
        start = manager->slots;
        off2 = 0x920;
        asm("" : "+r"(off2));
        mask = (u32 *)((u32)manager + off2);
        table = gUnk_08610728;
        state = &manager->slots[0].state;
        slot = start;
        for (; i < 32;) {
            if (Occupied(mask, i))
                sub_08249240(manager, slot, table[*state]);
            state += 0x48;
            slot = (void *)((u32)slot + 0x48);
            i++;
        }
    }
    return 0;
}
