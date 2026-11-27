#include "global.h"
struct HudEntry {
    u8 pad0[2];
    u8 isEquipment;
    u8 id;
    u8 pad[4];
};
struct HudSprite {
    u8 pad[8];
    u32 flags;
    u8 pad2[0x54];
};
struct SceneHud {
    u8 padDA8[0xDA8];
    struct HudEntry entries[16];
    struct HudSprite options[3];
    u8 padF48[0xF48 - 0xE28 - 0x120];
    u8 selection;
    u8 padF49[2];
    u8 visibleCount;
    u8 padF4C[0xF70 - 0xF4C];
    struct HudSprite sprites[12];
    u8 pad13F0[2];
    u8 count;
};
void PauseMenu_LoadEquipmentIconFrame(struct SceneHud *, void *, s32);
void PauseMenu_LoadCategoryIconFrame(struct SceneHud *, void *, s32);
void PauseMenu_LoadIndexedIconKeyframe(struct SceneHud *, void *, s32);
s32 PauseMenu_IsEquipmentSelectionEnabled(struct SceneHud *);
s32 PauseMenu_GetSelectedListEntryType(struct SceneHud *);
static inline void HudSprite_Hide(struct HudSprite *s)
{
    s->flags |= 1;
}
static inline void HudSprite_Show(struct HudSprite *s)
{
    s->flags &= ~1;
}
void SceneHud_RefreshSecondaryOptions(struct SceneHud *p)
{
    s32 i;
    struct HudSprite *sprite;
    s32 index;

    for (i = 0; i <= 2; i++) {
        index = i + (s8)p->selection;
        if (index >= p->count)
            index -= p->count;
        if (p->entries[index].isEquipment)
            PauseMenu_LoadEquipmentIconFrame(p, &p->options[i], p->entries[index].id);
        else
            PauseMenu_LoadCategoryIconFrame(p, &p->options[i], p->entries[index].id);
    }
    sprite = &p->sprites[5];
    PauseMenu_LoadIndexedIconKeyframe(p, sprite, (s8)p->selection);
    if (PauseMenu_IsEquipmentSelectionEnabled(p)) {
        HudSprite_Hide(&p->sprites[4]);
        HudSprite_Hide(&p->sprites[7]);
        HudSprite_Hide(&p->sprites[10]);
        sprite->flags |= 1;
        HudSprite_Hide(&p->sprites[8]);
        HudSprite_Hide(&p->sprites[11]);
    } else if (PauseMenu_GetSelectedListEntryType(p)) {
        HudSprite_Show(&p->sprites[4]);
        HudSprite_Show(&p->sprites[7]);
        HudSprite_Show(&p->sprites[10]);
        for (i = 0; i <= 2; i++) {
            if (i < p->visibleCount)
                HudSprite_Show(&p->sprites[i]);
            else
                HudSprite_Hide(&p->sprites[i]);
        }
    } else {
        HudSprite_Hide(&p->sprites[4]);
        HudSprite_Hide(&p->sprites[7]);
        HudSprite_Hide(&p->sprites[10]);
        for (i = 0; i <= 2; i++)
            p->sprites[i].flags |= 1;
    }
}
