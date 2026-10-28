#include "modifier_list.h"
#include "ram.h"
#include "area_effect_link.h"
void ActorSprite_InitPartDrawings(u8 *, u8 *, u32);
void ModifierMenu_RefreshPreviewModel(struct ModifierMenu *);
void CharacterPreview_PositionParts(u8 *, u8 *, struct ModifierPreviewOrigin *, u32);
void CharacterModel_SetAnimationFlags(u8 *, s32);
void CharacterModel_SetComponentRenderMode(u8 *, s32);
void CharacterModel_InitPalettes(u8 *, s32);
void ModifierMenu_InitializePreview(struct ModifierMenu *menu)
{
    struct ModifierPreviewOrigin origin;
    u8 *resource;
    u8 *model;
    u32 flags;
    u32 mask;
    origin.x = 48;
    origin.y = 80;
    origin.z = 0;
    mask = 0x800;
    flags = (gEngineFlags | gScenePauseFlags) & mask;
    if (flags) {
        s32 id;
        struct ModifierMenuActor *actor = gLinkSession;
        if (!actor) {
            id = -1;
        } else {
            *(&id) = actor->id;
        }
        menu->previewActorId = id;
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
