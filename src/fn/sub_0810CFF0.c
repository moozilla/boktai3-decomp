#include "global.h"
#include "area_effect_link.h"
struct LinkPreviewPart {
    u16 priority;
    u8 unknown2[0x5E];
};
struct LinkPreview {
    u8 unknown0[0x48];
    u8 kind, variant, form, palette, extra;
    u8 unknown4D[0x5F8 - 0x4D];
    u8 model[0x84A - 0x5F8];
    u8 player;
    u8 unknown84B[0x850 - 0x84B];
};
struct LinkPreviewPanel {
    u8 unknown0[0x348];
    struct LinkPreview previews[2];
};
struct LinkMenuPosition {
    u32 x : 16, y : 16, z : 16, padding : 16;
};
void Memory_Clear(void *, int);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterPreview_PositionParts(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void CharacterModel_InitPalettes(void *, s32);
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

/* Loads both players' character previews from the received link payloads. */
void sub_0810CFF0(struct LinkPreviewPanel *panel)
{
    s32 i;
    struct LinkMenuPosition pos;
    for (i = 0; i <= 1; i++) {
        struct AreaEffectLinkState *link = gLinkSession;
        u8 *payload;
        struct LinkPreview *preview;
        u8 *model;
        s32 flags;
        s32 k;
        if (!link || link->unavailable != 1 || !validSelection(i))
            return;
        payload = link->incomingPayload + i * 0x24;
        if (!payload)
            return;
        preview = &panel->previews[i];
        preview->kind = payload[0];
        preview->form = payload[2];
        preview->variant = payload[1];
        preview->extra = payload[4];
        preview->palette = payload[3];
        preview->player = i;
        switch (i) {
            case 0:
                flags = 0;
                pos.x = 0x30;
                pos.y = 0x68;
                pos.z = 0;
                break;
            case 1:
                flags = 4;
                pos.x = 0xC0;
                pos.y = 0x68;
                pos.z = 0;
                break;
            default:
                flags = 0;
                Memory_Clear(&pos, 8);
                break;
        }
        model = preview->model;
        ActorSprite_InitPartDrawings(preview, model, 0x3D99);
        CharacterPreview_PositionParts(preview, model, &pos, flags);
        CharacterModel_SetAnimationFlags(model, flags);
        CharacterModel_SetComponentRenderMode(model, 0);
        CharacterModel_InitPalettes(model, preview->palette);
        for (k = 0; k <= 4; k++)
            ((struct LinkPreviewPart *)(preview->model + 0x3A))[k].priority = i * 2 - 0x6000;
    }
}
