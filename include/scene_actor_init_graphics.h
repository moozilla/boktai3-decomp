#ifndef GUARD_SCENE_ACTOR_INIT_GRAPHICS_H
#define GUARD_SCENE_ACTOR_INIT_GRAPHICS_H

#include "scene_actor_init_actor.h"
#include "scene_actor_script_api.h"
#include "display_keyframe.h"
#include "display_resource.h"
#include "render.h"

s32 AnimData_Load(void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Sprite_Init(void *, void *, s32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void *ScriptProc_Start(u16, u16);
s32 Widget_InitLinked(u8 *, void *, u16, u32, u8, u8, u8, const struct ContactPosition *);

static inline void MonsterInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MonsterInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct DisplayResourceHeader *)drawable = *(struct DisplayResourceHeader *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void MonsterInit_SetWidgetPalette(struct DisplayKeyframeState *widget, u32 palette)
{
    widget->paletteIndex = palette;
}

static inline void MonsterInit_SetWidgetScriptPalette(struct DisplayKeyframeState *widget,
                                                      s32 scriptPalette)
{
    widget->paletteIndex = scriptPalette - 0x5FFF;
}

static inline void MonsterInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline void MonsterInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                           u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue('C', 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

#endif
