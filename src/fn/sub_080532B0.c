// MATCHING_CONCESSION: register-pinning
#include "stacked_actor.h"
#include "engine_globals.h"
struct ActorMapView {
    u8 unknown0[4];
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rowOffsets[1];
};
extern u8 *gUnk_02000580;
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
s32 StackedActor_Initialize(struct StackedActor *actor, s32 tag, s32 unused)
{
    struct InitCache {
        s32 tag;
        void *graphics;
        struct StackedActorPosition *world;
        struct StackedActorSprite *segments;
        void *receiver;
        u16 *proximity;
    } cache;
    register u16 *baseFrame asm("r10");
    u32 present;
    u32 mode;

    cache.tag = tag;
    present = Script_SeekToKeyword('p');
    if (present) {
        s32 x, z;
        s32 index;
        register s16 *worldX asm("r4");
        register s16 *worldY asm("r5");
        u8 *cell;
        u32 type;
        s32 y;
        u32 value = Script_GetValue();
        worldX = &actor->world.x;
        *worldX = value;
        value = Script_GetValue();
        worldY = &actor->world.y;
        *worldY = value;
        actor->world.z = Script_GetValue();
        x = (s16) * (volatile u16 *)worldX >> 8;
        z = (s16) * (volatile u16 *)((u8 *)worldX + 4) >> 8;
        cache.world = (void *)worldX;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else {
            u32 row = ((struct ActorMapView *)gTerrainMap)->rowOffsets[z];
            index = row + x;
        }
        cell = TerrainModifiers_FindCellOverride(index, 1);
        if (cell)
            cell += 4;
        else {
            struct ActorMapView *map = ((struct ActorMapView *)gTerrainMap);
            u32 offset = index * 4 + 12;
            cell = map->cells + offset;
        }
        type = *cell >> 4;
        y = (*cell & 15) << 8;
        switch (type) {
            case 1:
                y -= *((u8 *)worldX + 4);
                break;
            case 2:
                y -= *(u8 *)worldX;
                break;
        }
        *worldY = y;
    } else {
        actor->world.x = present;
        actor->world.y = present;
        actor->world.z = present;
        cache.world = &actor->world;
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
    baseFrame = &actor->baseFrame;
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
    {
        register u16 *source asm("r1") = baseFrame;
        actor->paletteBase = (*source >> 1) * 3 + 160;
    }
    {
        void *graphics = actor->graphics;
        u16 *tiles = &actor->paletteBase;
        struct StackedActorSprite *sprite;
        void *shadowGraphics;
        struct StackedActorSprite *shadow;
        register struct StackedActorPosition *shadowPosition asm("r2");
        AnimData_Load(graphics, 0xBA8);
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 0);
        {
            register u16 *source asm("r1") = baseFrame;
            sprite->frame = *source;
        }
        AnimData_SelectPaletteBank(graphics, *tiles);
        actor->sprite.flags |= 0x200;
        {
            u8 *destination = &actor->sprite.renderMode;
            register u32 renderMode asm("r2") = mode;
            *destination = renderMode;
        }
        {
            register struct StackedActorPosition *source asm("r3") = cache.world;
            actor->sprite.position = *source;
        }
        shadowGraphics = actor->shadowGraphics;
        AnimData_Load(shadowGraphics, 0xBA8);
        shadow = &actor->shadow;
        Sprite_Init(shadow, shadowGraphics, 0);
        {
            register u16 *source asm("r1") = baseFrame;
            shadow->frame = *source;
        }
        AnimData_SelectPaletteBank(shadowGraphics, *tiles);
        shadow->flags |= 0x400;
        {
            u8 *destination = &actor->shadow.renderMode;
            register u32 renderMode asm("r2") = mode;
            *destination = renderMode;
        }
        shadowPosition = &actor->shadow.position;
        {
            register struct StackedActorPosition *source asm("r3") = cache.world;
            *shadowPosition = *source;
        }
        {
            s32 i = 0;
            s32 count;
            register u32 one asm("r5");
            struct StackedActorSprite *savedShadow;
            u8 *segmentGraphics;
            cache.graphics = graphics;
            savedShadow = shadow;
            cache.segments = (void *)((u8 *)shadowPosition + 0x2C);
            {
                register void *receiver asm("r3") = actor->trigger;
                u16 *state;
                cache.receiver = receiver;
                state = &actor->proximityState;
                cache.proximity = state;
            }
            {
                register u32 offset asm("r1") = 0x16C;
                asm("" : : "r"(offset));
                segmentGraphics = (void *)((u8 *)actor + offset);
            }
            do {
                register u32 offset asm("r4");
                struct StackedActorSprite *current;
                AnimData_Load(segmentGraphics, 0xBA8);
                {
                    u32 stride = sizeof(struct StackedActorSprite);
                    offset = i;
                    offset *= stride;
                }
                {
                    u32 currentOffset = offset;
                    currentOffset += 0xE8;
                    current = (void *)((u8 *)actor + currentOffset);
                }
                Sprite_Init(current, segmentGraphics, 0);
                {
                    register u16 *source asm("r2") = baseFrame;
                    u32 frame = *source;
                    register u32 selected asm("r1");
                    one = 1;
                    selected = one;
                    selected &= frame;
                    current = (void *)(offset + (u32)actor + 0xE8);
                    selected += 16;
                    current->frame = selected;
                }
                {
                    register u32 tileOffset asm("r3") = 0x1C4;
                    register u16 *tiles asm("r0");
                    asm("" : : "r"(tileOffset));
                    tiles = (void *)((u8 *)actor + tileOffset);
                    AnimData_SelectPaletteBank(segmentGraphics, *tiles + 2);
                }
                {
                    register struct StackedActorSprite *base asm("r0") = cache.segments;
                    register u32 *flags asm("r2") = (void *)((u8 *)base + offset);
                    *flags |= 0x201;
                }
                {
                    u8 *relative = (u8 *)actor + offset;
                    u8 *destination = relative + 0xEF;
                    register u32 renderMode asm("r2") = 1;
                    *destination = renderMode;
                    {
                        u32 offset = 0x104;
                        struct StackedActorPosition *destination;
                        register struct StackedActorPosition *source asm("r3");
                        destination = (void *)(relative + offset);
                        source = cache.world;
                        *destination = *source;
                    }
                }
                segmentGraphics += 0x1C;
                {
                    u32 step = 1;
                    i += step;
                }
                count = i;
            } while (count <= 2);
            StackedActor_InitParticles(actor);
            SceneMessages_RegisterReceiver(cache.receiver, cache.tag, 10);
            if (Script_SeekToKeyword('D')) {
                register u32 zeroByte asm("r2") = 0;
                u32 zeroHalf = 0;
                register u16 *state asm("r3") = cache.proximity;
                *state = zeroHalf;
                actor->animationPhase = zeroHalf;
                actor->interactionDelay = zeroHalf;
                actor->lift = zeroByte;
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
                    actor->unknown380 = one;
                    ScriptUtility_SetControlFlag(0);
                    Player_SetPositionAndSave(gUnk_02000580, cache.world);
                    PlayerCue_Hide(gUnk_02000580);
                    actor->sprite.flags &= ~1;
                    {
                        u32 tile = actor->paletteBase + 2;
                        AnimData_SelectPaletteBank(cache.graphics, tile);
                    }
                    {
                        register struct StackedActorSprite *target asm("r1") = savedShadow;
                        target->flags |= one;
                    }
                    {
                        register u32 offset asm("r2") = 0x1C0;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 7;
                    }
                    StackedActor_UpdateSegments(actor);
                    {
                        register u32 offset asm("r3") = 0x1C6;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 8;
                    }
                    *cache.proximity = one;
                    {
                        register u32 offset asm("r2") = 0x36A;
                        register u16 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 10;
                    }
                    {
                        register u32 offset asm("r3") = 0x368;
                        register u8 *destination asm("r1");
                        asm("" : : "r"(offset));
                        destination = (void *)((u8 *)actor + offset);
                        *destination = 24;
                    }
                    actor->removeAfterUse = appear;
                    StackedActor_SetUpdate(actor, StackedActor_WaitForPlayerRelease);
                    tether = 0;
                    if (Script_SeekToKeyword('w'))
                        tether = Script_GetValue();
                    if (tether) {
                        if (!TetherActor_BeginKindFourStateTen())
                            return 0;
                        TetherActor_SetPosition(cache.world);
                    }
                } else {
                    {
                        register u32 zeroByte asm("r0") = 0;
                        register u16 *state asm("r2") = cache.proximity;
                        *state = active;
                        {
                            register u32 offset asm("r3") = 0x1C6;
                            register u16 *destination asm("r1");
                            asm("" : : "r"(offset));
                            destination = (void *)((u8 *)actor + offset);
                            *destination = active;
                        }
                        {
                            u32 offset = 0x36A;
                            u16 *destination;
                            destination = (void *)((u8 *)actor + offset);
                            *destination = active;
                        }
                        {
                            u32 offset = 0x368;
                            u8 *destination;
                            destination = (void *)((u8 *)actor + offset);
                            *destination = zeroByte;
                        }
                    }
                    if (appear) {
                        Sound_PlaySong(0x15E);
                        actor->sprite.flags |= 2;
                        {
                            struct StackedActorSprite *target = savedShadow;
                            target->flags |= 2;
                        }
                        actor->sprite.scaleX = one;
                        actor->sprite.scaleY = one;
                        actor->shadow.scaleX = one;
                        actor->shadow.scaleY = one;
                        StackedActor_SetUpdate(actor, StackedActor_Appear);
                    } else
                        StackedActor_SetUpdate(actor, StackedActor_TickIdle);
                }
            }
        }
    }
    gStackedActor = actor;
    return 0;
}
