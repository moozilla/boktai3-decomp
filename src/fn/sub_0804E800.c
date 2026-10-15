#include "area_effect.h"
// MATCHING_CONCESSION: register-pinning
#define AreaEffect_ShowEnabledSprites sub_0804E800
void sub_0804E758(struct AreaEffectManager *);
void AreaEffect_ShowEnabledSprites(struct AreaEffectManager *manager)
{
    if (!manager->visible) {
        s32 i;
        register u8 *count asm("r6");
        manager->visible = 1;
        i = 0;
        {
            u8 *cursor = &manager->count;
            asm("" : "+r"(cursor));
            count = cursor;
        }
        if (i < *count) {
            u8 *enabled = &manager->entries[0].enabled;
            s32 offset = 0;
            u8 *initialBase = (u8 *)manager + 0x34;
            u8 *base = initialBase;
            do {
                if (enabled[3] && enabled[0]) {
                    register u32 *flags asm("r0") = (u32 *)(base + offset);
                    u32 value = *flags;
                    register s32 mask asm("r2") = ~1;
                    *flags = value & mask;
                }
                enabled += 0xA8;
                offset += 0xA8;
                i++;
            } while (i < *count);
        }
    }
    sub_0804E758(manager);
    {
        struct AreaEffectPlayer *player = manager->player;
        u32 mask = 32;
        u32 flags = player->flags;
        u32 mode;
        if (flags & mask)
            mode = 2;
        else {
            u32 negative = 0 - (flags & 16);
            mode = negative >> 31;
        }
        sub_0804E618(manager, mode);
    }
}
