#define gLinkSession               gUnk_020004B4
#define MenuSprite_LoadPalettePair CharacterModel_InitPalettes
#include "global.h"
#include "area_effect_link.h"
struct LinkMenu {
    u8 unknown0[0x794];
    u8 sprite[0x48];
    u8 kind, variant, form, palette, extra;
    u8 unknown7E1[0xFFC - 0x7E1];
    s32 selected;
};
struct LinkMenuPosition {
    u32 x : 16, y : 16, z : 16, padding : 16;
};
void sub_0811C9F8(void *, void *, u32);
void sub_0811CB68(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void CharacterModel_InitPalettes(void *, s32);
static inline s32 localPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 validSelection(s32 i)
{
    if (i < 0)
        goto no;
    if (i <= 1)
        goto yes;
no:
    return 0;
yes:
    return 1;
}
void LinkMenu_LoadSelectionSprite(struct LinkMenu *menu)
{
    s32 i = menu->selected;
    struct AreaEffectLinkState *link = gUnk_020004B4;
    if (link && link->unavailable == 1 && validSelection(i)) {
        u8 *payload = link->incomingPayload + i * 0x24;
        if (payload) {
            u8 *sprite = menu->sprite;
            u8 *render;
            struct LinkMenuPosition pos;
            menu->kind = payload[0];
            menu->form = payload[2];
            menu->variant = payload[1];
            menu->extra = payload[4];
            menu->palette = payload[3];
            sprite[0x84A] = localPlayer();
            pos.x = 0x30;
            pos.y = 0x50;
            pos.z = 0;
            render = sprite + 0x5F8;
            sub_0811C9F8(sprite, render, 0x3D99);
            sub_0811CB68(sprite, render, &pos, 0);
            CharacterModel_SetAnimationFlags(render, 0);
            CharacterModel_SetComponentRenderMode(render, 0);
            CharacterModel_InitPalettes(render, payload[3]);
        }
    }
}
