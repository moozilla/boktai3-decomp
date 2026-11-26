#include "objects/warp_circle/stacked_actor.h"
#include "engine/engine_globals.h"
#include "objects/warp_circle/engine_map.h"
#include "player/player.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *TerrainModifiers_FindCellOverride(u32, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void StackedActor_InitParticles(struct StackedActor *);
s32 SceneMessages_RegisterReceiver(void *, s32, s32);
void Sound_PlaySong(u32);
void ScriptUtility_SetControlFlag(u32);
void Player_SetPositionAndSave(u8 *, struct StackedActorPosition *);
void PlayerCue_Hide(u8 *);
void StackedActor_TickDisabled(struct StackedActor *);
void StackedActor_Appear(struct StackedActor *);
void StackedActor_WaitForPlayerRelease(struct StackedActor *);
s32 TetherActor_BeginKindFourStateTen(void);
void TetherActor_SetPosition(struct StackedActorPosition *);
static inline void StackedActorSprite_SetFrame(struct StackedActorSprite *sprite, u32 frame)
{
    sprite->frame = frame;
}
s32 StackedActor_Initialize(struct StackedActor *actor, s32 tag, s32 unused)
{
    u32 mode;
    s32 i;
    u8 *graphics;
    if (Script_SeekToKeyword('p')) {
        u32 cell;
        u32 height, surface;
        u8 *properties;
        struct StackedActorPosition *point;
        s32 x, z;
        actor->world.x = Script_GetValue();
        actor->world.y = Script_GetValue();
        actor->world.z = Script_GetValue();
        point = &actor->world;
        x = point->x >> 8;
        z = point->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            cell = 0;
        else {
            u32 row = gTerrainMap->rows[z];
            cell = row + x;
        }
        properties = TerrainModifiers_FindCellOverride(cell, 1);
        if (properties)
            properties += 4;
        else {
            struct EngineMap *map = gTerrainMap;
            u32 offset = cell * 4 + 12;
            properties = map->cells + offset;
        }
        surface = *properties >> 4;
        height = (*properties & 15) << 8;
        switch (surface) {
            case 1:
                height -= (u8)point->z;
                break;
            case 2:
                height -= (u8)point->x;
                break;
        }
        actor->world.y = height;
    } else {
        actor->world.x = 0;
        actor->world.y = 0;
        actor->world.z = 0;
    }
    if (Script_SeekToKeyword('C'))
        actor->pendingScriptId = Script_GetValue();
    else
        actor->pendingScriptId = 0;
    if (Script_SeekToKeyword('c'))
        actor->acceptedScriptId = Script_GetValue();
    else
        actor->acceptedScriptId = 0;
    if (Script_SeekToKeyword('i'))
        actor->completedScriptId = Script_GetValue();
    else
        actor->completedScriptId = 0;
    if (Script_SeekToKeyword('o'))
        actor->particleScriptId = Script_GetValue();
    else
        actor->particleScriptId = 0;
    if (Script_SeekToKeyword('d'))
        actor->baseFrame = Script_GetValue();
    else
        actor->baseFrame = 6;
    if (Script_SeekToKeyword('a'))
        actor->interactionEnabled = Script_GetValue();
    else
        actor->interactionEnabled = 0;
    if (Script_SeekToKeyword('n'))
        actor->particleScriptValue = Script_GetValue();
    else
        actor->particleScriptValue = 0;
    if (Script_SeekToKeyword('P'))
        mode = Script_GetValue();
    else
        mode = 3;
    actor->paletteBase = (actor->baseFrame >> 1) * 3 + 160;
    AnimData_Load(actor->graphics, 0xBA8);
    {
        struct StackedActorSprite *sprite = &actor->sprite;
        Sprite_Init(sprite, actor->graphics, 0);
        sprite->frame = actor->baseFrame;
    }
    AnimData_SelectPaletteBank(actor->graphics, actor->paletteBase);
    actor->sprite.flags |= 0x200;
    actor->sprite.renderMode = mode;
    actor->sprite.position = actor->world;
    AnimData_Load(actor->shadowGraphics, 0xBA8);
    {
        struct StackedActorSprite *shadow = &actor->shadow;
        Sprite_Init(shadow, actor->shadowGraphics, 0);
        shadow->frame = actor->baseFrame;
    }
    AnimData_SelectPaletteBank(actor->shadowGraphics, actor->paletteBase);
    actor->shadow.flags |= 0x400;
    actor->shadow.renderMode = mode;
    actor->shadow.position = actor->world;
    i = 0;
    graphics = actor->graphics;
    do {
        AnimData_Load(actor->segmentGraphics[i], 0xBA8);
        Sprite_Init(&actor->segments[i], actor->segmentGraphics[i], 0);
        StackedActorSprite_SetFrame(&actor->segments[i], (actor->baseFrame & 1) + 16);
        AnimData_SelectPaletteBank(actor->segmentGraphics[i], actor->paletteBase + 2);
        actor->segments[i].flags |= 0x201;
        actor->segments[i].renderMode = 1;
        actor->segments[i].position = actor->world;
    } while (++i < 3);
    StackedActor_InitParticles(actor);
    SceneMessages_RegisterReceiver(actor->trigger, tag, 10);
    if (Script_SeekToKeyword('D')) {
        actor->proximityState = 0;
        actor->animationPhase = 0;
        actor->interactionDelay = 0;
        actor->lift = 0;
        StackedActor_SetUpdate(actor, StackedActor_TickDisabled);
    } else {
        s32 active = 0;
        s32 appear;
        if (Script_SeekToKeyword('f'))
            active = Script_GetValue();
        appear = 0;
        if (Script_SeekToKeyword('s'))
            appear = Script_GetValue();
        if (active) {
            s32 tether;
            Sound_PlaySong(0x160);
            actor->unknown380 = 1;
            ScriptUtility_SetControlFlag(0);
            Player_SetPositionAndSave(*(u8 **)gPlayers, &actor->world);
            PlayerCue_Hide(*(u8 **)gPlayers);
            actor->sprite.flags &= ~1;
            AnimData_SelectPaletteBank(graphics, actor->paletteBase + 2);
            actor->shadow.flags |= 1;
            actor->height = 7;
            StackedActor_UpdateSegments(actor);
            actor->animationPhase = 8;
            actor->proximityState = 1;
            actor->interactionDelay = 10;
            actor->lift = 24;
            actor->removeAfterUse = appear;
            StackedActor_SetUpdate(actor, StackedActor_WaitForPlayerRelease);
            tether = 0;
            if (Script_SeekToKeyword('w'))
                tether = Script_GetValue();
            if (tether) {
                if (!TetherActor_BeginKindFourStateTen())
                    return 0;
                TetherActor_SetPosition(&actor->world);
            }
        } else {
            actor->proximityState = 0;
            actor->animationPhase = 0;
            actor->interactionDelay = 0;
            actor->lift = 0;
            if (appear) {
                Sound_PlaySong(0x15E);
                actor->sprite.flags |= 2;
                actor->shadow.flags |= 2;
                actor->sprite.scaleX = 1;
                actor->sprite.scaleY = 1;
                actor->shadow.scaleX = 1;
                actor->shadow.scaleY = 1;
                StackedActor_SetUpdate(actor, StackedActor_Appear);
            } else
                StackedActor_SetUpdate(actor, StackedActor_TickIdle);
        }
    }
    gStackedActor = actor;
    return 0;
}
