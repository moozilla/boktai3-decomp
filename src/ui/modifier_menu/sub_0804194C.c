#include "engine/engine_pause.h"
#include "scene/actor/modifier_list.h"
#include "engine/ram.h"
#include "scene/area_effect/area_effect_link.h"
void ActorSprite_InitPartDrawings(u8 *, u8 *, u32);
void ModifierMenu_RefreshPreviewModel(struct ModifierMenu *);
void CharacterPreview_PositionParts(u8 *, u8 *, struct ModifierPreviewOrigin *, u32);
void CharacterModel_SetAnimationFlags(u8 *, s32);
void CharacterModel_SetComponentRenderMode(u8 *, s32);
void CharacterModel_InitPalettes(u8 *, s32);
static inline s16 ModifierMenu_GetPreviewActorId(struct ModifierMenuActor *actor)
{
    if (!actor)
        return -1;
    return actor->id;
}
void ModifierMenu_InitializePreview(struct ModifierMenu *menu)
{
    struct ModifierPreviewOrigin origin;
    u8 *resource;
    u8 *model;
    u32 flags;
    origin.x = 48;
    origin.y = 80;
    origin.z = 0;
    flags = Engine_PauseBits(0x800);
    if (flags) {
        menu->previewActorId = ModifierMenu_GetPreviewActorId(gLinkSession);
    } else {
        menu->previewActorId = flags;
    }
    resource = (u8 *)menu + 0x83C;
    model = (u8 *)menu + 0xE34;
    ActorSprite_InitPartDrawings(resource, model, 0x3D99);
    ModifierMenu_RefreshPreviewModel(menu);
    CharacterPreview_PositionParts(resource, model, &origin, 0);
    CharacterModel_SetAnimationFlags(model, 16);
    CharacterModel_SetComponentRenderMode(model, 1);
    CharacterModel_InitPalettes(model, gGameState->unk81A);
}
