#include "global.h"
#include "link/lobby/record_menu.h"

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

void RecordMenu_LoadPlayerSprites(struct RecordMenu *menu)
{
    s32 i;
    struct SceneInputPackedPosition pos;

    for (i = 0; i < 2; i++) {
        struct AreaEffectLinkState *link = gLinkSession;
        struct LinkRecordIncomingStyle *payload;
        struct CharacterPreviewActor *preview;
        struct CharacterPreviewModel *model;
        s32 flags;
        s32 k;

        if (!link || link->unavailable != 1 || !validSelection(i))
            return;
        payload = &((struct LinkRecordIncomingStyle *)link->incomingPayload)[i];
        if (!payload)
            return;
        preview = &menu->previews[i];
        preview->kind = payload->kind;
        preview->form = payload->form;
        preview->variant = payload->variant;
        preview->extra = payload->extra;
        preview->palette = payload->palette;
        preview->model.playerIndex = i;
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
        model = &preview->model;
        ActorSprite_InitPartDrawings(preview, model, 0x3D99);
        CharacterPreview_PositionParts(preview, model, &pos, flags);
        CharacterModel_SetAnimationFlags(model, flags);
        CharacterModel_SetComponentRenderMode(model, 0);
        CharacterModel_InitPalettes(model, payload->palette);
        for (k = 0; k < 5; k++)
            preview->model.view.parts[k].tile = i * 2 - 0x6000;
    }
}
