#ifndef GUARD_CHARACTER_MODEL_H
#define GUARD_CHARACTER_MODEL_H
#include "modifier_list.h"
#include "scene_pool.h"
#include "actor_stats.h"
#include "scene_actor_style.h"
#include "area_effect_link.h"
#include "tilemap_panel.h"
#include "background_controller.h"
/* Sixteen colors followed by a two-frame rotation timer. */
struct CharacterPaletteCycle {
    u16 colors[16], timer;
};
struct CharacterPaletteSource {
    u8 unknown0[0x20];
    u16 colors[16];
};
s32 Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, s32);
s32 ActorResource_SelectElementResource(s32);
s32 CharacterModel_GetPrimaryPaletteIndex(struct CharacterPreviewModel *);
void CharacterModel_LoadPrimaryPalette(struct CharacterPreviewModel *);
void PaletteFlash_SelectPalette(struct CharacterPreviewPalette *, s32);
void ScenePalette_RefreshOnFlags(struct ScenePoolActor *, struct CharacterPreviewModel *);
u8 *Video_GetWrappedBackgroundMapEntry(u32, u32, u32);
s32 ModifierMenu_CountAppearanceModifiers(struct SceneActorStyle *);
void Character_SetLowResourceColorEffect(void *, void *);
s32 ModifierStats_ApplyKinds4And14(void *, s32);
void Modifier_StartDrainIfAboveThreshold(void *);
s32 ModifierStats_ApplyKind0(void *, s32);
void Actor_SetSpriteDepthOffsets(void *, s32);
u32 GameState_HasEquippedConsumable(s32);
u8 SceneSlot_RegisterObject(u32);
u8 DisplaySlots_Reserve(u32, u32);
const u8 *Script_ReadString(const u8 *);
#endif
