#include "scene/collision/scene_actor_damage_visibility.h"
#include "global.h"
#include "link/records/link_record_panel.h"
#include "ui/save/save_menu.h"
struct LinkMenuPosition {
    u32 x : 16, y : 16, z : 16, padding : 16;
};
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
void LinkMenu_LoadSelectionSprite(struct SaveMenu *menu)
{
    s32 i = menu->localPlayer;
    struct AreaEffectLinkState *link = gLinkSession;
    if (link && link->unavailable == 1 && validSelection(i)) {
        struct LinkRecordIncomingStyle *payload =
            &((struct LinkRecordIncomingStyle *)link->incomingPayload)[i];
        if (payload) {
            struct CharacterPreviewActor *preview = &menu->preview;
            struct LinkMenuPosition pos;
            preview->kind = payload->kind;
            preview->form = payload->form;
            preview->variant = payload->variant;
            preview->extra = payload->extra;
            preview->palette = payload->palette;
            preview->model.playerIndex = SceneActorDamage_LocalPlayer();
            pos.x = 0x30;
            pos.y = 0x50;
            pos.z = 0;
            ActorSprite_InitPartDrawings(preview, &preview->model, 0x3D99);
            CharacterPreview_PositionParts(preview, &preview->model, &pos, 0);
            CharacterModel_SetAnimationFlags(&preview->model, 0);
            CharacterModel_SetComponentRenderMode(&preview->model, 0);
            CharacterModel_InitPalettes(&preview->model, payload->palette);
        }
    }
}
