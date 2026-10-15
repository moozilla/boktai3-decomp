#include "area_effect_link.h"
// MATCHING_CONCESSION: register-pinning
#define AreaEffect_UpdateLinkVisibility sub_0804EDD0
static inline s32 localPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u32 linkValue(s32 index)
{
    if (!gUnk_020004B4 || index < 0)
        return 0;
    return gUnk_020004B4->playerValues[index];
}
void AreaEffect_UpdateLinkVisibility(struct AreaEffectManager *manager)
{
    struct AreaEffectManager *object = manager;
    register u32 sample asm("r0") = linkValue(localPlayer());
    u32 value;
    asm("" : "+r"(sample));
    value = sample;
    if (value) {
        u8 *visibility = &object->visible;
        if (!*visibility) {
            s32 i;
            register u8 *count asm("r5");
            *visibility = 1;
            i = 0;
            {
                u8 *cursor = &object->count;
                asm("" : "+r"(cursor));
                count = cursor;
            }
            if (i < *count) {
                u8 *enabled = &object->entries[0].enabled;
                u32 offset = 0;
                s32 countValue;
                u8 *base = (u8 *)object + 0x34;
                do {
                    if (enabled[3] && *enabled) {
                        register u32 *flags asm("r0") = (u32 *)(base + offset);
                        u32 old = *flags;
                        register u32 mask asm("r2") = ~1;
                        *flags = old & mask;
                    }
                    enabled += 0xA8;
                    offset += 0xA8;
                    i++;
                    countValue = *count;
                } while (i < countValue);
            }
        }
    } else {
        u8 *visibility = &object->visible;
        if (*visibility == 1) {
            s32 i;
            register u8 *count asm("r5");
            *visibility = value;
            i = 0;
            {
                u8 *cursor = &object->count;
                asm("" : "+r"(cursor));
                count = cursor;
            }
            if (i < *count) {
                u32 mask = 1;
                u8 *hasSprite = &object->entries[0].hasSprite;
                u8 *base = (u8 *)object;
                register u8 *readCount asm("r3") = count;
                do {
                    if (*hasSprite)
                        *(u32 *)(base + 0x34) |= mask;
                    hasSprite += 0xA8;
                    base += 0xA8;
                    i++;
                } while (i < *readCount);
            }
        }
    }
}
