#include "global.h"
#include "area_effect_link.h"
struct EffectQuadPosition {
    u32 x, y;
};
struct EffectQuadEntry {
    u8 state, unknown1, active, unknown3;
    u32 frame;
    u8 drawable[0x28];
    struct EffectQuadPosition position;
};
struct EffectQuad {
    u8 unknown0[0x20];
    struct EffectQuadEntry entries[4];
    void *resource;
};
extern struct EffectQuad *gEffectQuad;
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
void EffectQuad_StartRemoteEntry(s32 player, const struct EffectQuadPosition *position)
{
    if (gEffectQuad) {
        if (LocalPlayer() != player) {
            struct EffectQuadEntry *entry = &gEffectQuad->entries[player];
            entry->position = *position;
            SpriteDrawable_SelectTileFrame(entry->drawable, gEffectQuad->resource, 0x33);
            {
                u32 one = 1;
                u32 zero = 0;
                entry->state = one;
                entry->frame = zero;
                entry->active = one;
            }
        }
    }
}
