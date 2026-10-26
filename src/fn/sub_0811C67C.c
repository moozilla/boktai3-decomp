#define Equipment_HasCategory GameState_HasEquippedConsumable
#define gAreaEffectLinkState  gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
#include "engine.h"
#include "ram.h"
u32 GameState_HasEquippedConsumable(s32);
static inline u8 ValidIndex(s32 index)
{
    if (index < 0)
        goto invalid;
    if (index <= 1)
        goto valid;
invalid:
    return 0;
valid:
    return 1;
}
s32 ActorResource_SelectElementResource(s32 index)
{
    u32 kind;
    s32 count, selected;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        struct AreaEffectLinkState *link = gUnk_020004B4;
        struct AreaEffectLinkResourceSlot *slot;
        if (!link || link->unavailable != 1 || !ValidIndex(index))
            return 0x378;
        slot = (void *)((u8 *)link + (index * 0x40 + 0x120));
        if (!slot)
            return 0x378;
        kind = slot->element;
    } else {
        count = 0;
        if (GameState_HasEquippedConsumable(10)) {
            selected = 2;
            count = 1;
        }
        if (GameState_HasEquippedConsumable(26)) {
            selected = 3;
            count++;
        }
        if (GameState_HasEquippedConsumable(37)) {
            selected = 4;
            count++;
        }
        if (GameState_HasEquippedConsumable(47)) {
            selected = 5;
            count++;
        }
        kind = 1;
        if (count <= 3) {
            kind = 0;
            if (count == 1)
                kind = selected;
        }
    }
    switch (kind) {
        case 1:
            return 0x10e;
        case 2:
            return 0x10b;
        case 3:
            return 0x10d;
        case 4:
            return 0x10c;
        case 5:
            return 0x10a;
        case 0:
        default:
            return 0x378;
    }
}
