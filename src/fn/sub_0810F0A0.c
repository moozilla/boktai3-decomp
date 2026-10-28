#include "global.h"
#include "area_effect_link.h"
struct RecSlotOrigin {
    u32 x : 16, y : 16, z : 16, padding : 16;
};
struct RecSlot {
    u8 unknown0[0x48];
    u8 kind, variant, form, palette, extra;
    u8 unknown4D[0x5F8 - 0x4D];
    u8 model[0x84A - 0x5F8];
    u8 player;
    u8 unknown84B[0x850 - 0x84B];
};
struct RecMenu {
    u8 unknown0[0x438];
    struct RecSlot slots[2];
};
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterPreview_PositionParts(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void CharacterModel_InitPalettes(void *, s32);
void Memory_Clear(void *, u32);
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
void RecordMenu_LoadPlayerSprites(struct RecMenu *menu)
{
    s32 i, next;
    for (i = 0; i <= 1; i = next) {
        struct AreaEffectLinkState *link = gLinkSession;
        u8 *payload;
        struct RecSlot *slot;
        struct RecSlotOrigin origin;
        u8 *model;
        s32 mode;
        u32 v;
        u32 v2;
        s32 k;
        u16 *p;
        if (!link || link->unavailable != 1 || !validSelection(i))
            return;
        payload = link->incomingPayload + i * 0x24;
        if (!payload)
            return;
        slot = &menu->slots[i];
        slot->kind = payload[0];
        slot->form = payload[2];
        slot->variant = payload[1];
        k = payload[4];
        slot->extra = k;
        slot->palette = payload[3];
        slot->player = i;
        switch (i) {
            case 0:
                mode = 0;
                origin.x = 0x30;
                origin.y = 0x68;
                origin.z = 0;
                break;
            case 1:
                mode = 4;
                origin.x = 0xC0;
                origin.y = 0x68;
                origin.z = 0;
                break;
            default:
                mode = 0;
                Memory_Clear(&origin, 8);
                break;
        }
        model = slot->model;
        ActorSprite_InitPartDrawings(slot, model, 0x3D99);
        CharacterPreview_PositionParts(slot, model, &origin, mode);
        CharacterModel_SetAnimationFlags(model, mode);
        CharacterModel_SetComponentRenderMode(model, 0);
        CharacterModel_InitPalettes(model, payload[3]);
        v2 = i * 2;
        next = i + 1;
        v = v2 + 0xFFFFA000;
        k = 4;
        p = (u16 *)((u8 *)slot + 0x7B2);
        for (; k >= 0; k--) {
            *p = v;
            p -= 0x30;
        }
    }
}
