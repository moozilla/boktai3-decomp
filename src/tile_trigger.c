#include "tile_trigger.h"
#include "scene_actor_chase.h"
#include "ram.h"
#include "proximity_emitter.h"
#include "engine.h"
#include "engine_globals.h"
#include "display_animation.h"
#include "display_sprite_playback.h"
#include "display_sequence_inline.h"

/* Tentative trigger payload view, established by the two script-call paths. */

void ScriptTriggerActor_DispatchPendingScripts(struct TileTrigger *actor)
{
    s32 values[8];
    struct TileTriggerScriptArgs args;
    if (actor->triggerFlags & 0x100) {
        Memory_Clear(values, 0x20);
        if (actor->oneShotScript) {
            values[0] = actor->key;
            values[1] = actor->oneShotArguments[0];
            values[2] = actor->oneShotArguments[1];
            args.count = 3;
            args.values = values;
            Script_Call(actor->oneShotScript, &args);
            actor->oneShotScript = 0;
        }
    }
    if (actor->triggerFlags & 0x80) {
        Memory_Clear(values, 0x20);
        if (actor->pendingScript) {
            values[0] = actor->key;
            values[1] = actor->arguments[0];
            values[2] = actor->arguments[1];
            values[3] = actor->arguments[2];
            values[4] = actor->arguments[3];
            values[5] = actor->arguments[4];
            values[6] = actor->arguments[5];
            values[7] = (actor->triggerFlags >> 10) & 1;
            args.count = 8;
            args.values = values;
            Script_Call(actor->pendingScript, &args);
            actor->pendingScript = 0;
        }
    }
    actor->triggerFlags &= 0xFA7F;
}

struct Player;

struct EffectGroup;

struct TileTrigger;

struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *, const struct SceneVec *,
                                                  s32);

struct SceneActorRegistryActor *sub_0806F06C(const struct ContactPosition *,
                                             const struct ContactPosition *, u32 *);

s32 Player_BeginDirectionalCue(struct Player *, s32);

void TetherActor_BeginDetachedState(u32);

s32 EffectGroup_SpawnPart(struct EffectGroup *, u32, u32, u32, u32, u32, u32);

void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);

static inline u32 ContactActor_ReadMotionFlags(const struct SceneActorRegistryResetView *actor,
                                               u32 mask)
{
    return mask & actor->flags2D4;
}

static inline u8 ContactActor_IsControlled(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}

static inline u32 TileContact_NextParticleSample(u32 *cursor, const u16 *table)
{
    u32 index = (*cursor + 1) & 1023;
    *cursor = index;
    return table[index];
}

static inline void TileContact_EmitThreeParts(struct SceneActorRegistryActor *actor)
{
    s32 i;
    u16 sample;
    const void *source;
    struct ContactPackedVector velocity, scale, position;
    for (i = 0; i < 3; i++) {
        source = gRandomTable;
        sample = TileContact_NextParticleSample(&gRandomIndex, source);
        velocity.x = (sample & 15) - 7;
        velocity.y = (TileContact_NextParticleSample(&gRandomIndex, source) & 31) - 16;
        velocity.z = (TileContact_NextParticleSample(&gRandomIndex, source) & 15) - 7;
        scale.x = 5;
        scale.y = 10;
        scale.z = 5;
        position.x = actor->effectOrigin->x;
        position.y = actor->effectOrigin->y + 128;
        position.z = actor->effectOrigin->z;
        source = actor->effectConfig;
        EffectGroup_SpawnPart((struct EffectGroup *)source, 3, (u32)&position, (u32)&velocity,
                              (u32)&scale, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}

void TileTrigger_ApplyContactEffects(struct TileTrigger *self)
{
    struct Player *players[4];
    u8 inRange[4];
    struct ContactPackedVector radius;
    u32 cursor;
    s32 tetherInRange, actorFound, i;
    struct SceneActorRegistryResetView *actor;
    void (*callback)(struct SceneActorRegistryResetView *, s32);

    callback = SceneActorReaction_SinkAndRemove;
    cursor = 0;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 0; i < 4; i++)
        inRange[i] = 0;
    actorFound = 0;
    tetherInRange = 0;
    for (i = 0; i < 4; i++) {
        players[i] = SceneActorQuery_FindPlayerInBounds(&self->position,
                                                        (const struct SceneVec *)&radius, i);
        if (players[i]) {
            inRange[i] = 1;
            self->triggerFlags |= 0x80;
        }
    }
    if (TileTrigger_IsTetherNearby(self)) {
        tetherInRange = 1;
        self->triggerFlags |= 0x100;
    }
    while ((actor = (void *)sub_0806F06C((const struct ContactPosition *)&self->position,
                                         (const struct ContactPosition *)&radius, &cursor)) !=
           NULL) {
        if ((u8)(actor->field2B5 - 0x16) <= 1)
            continue;
        {
            u8 kind = 0x16;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *field = &actor->flags114;
                u32 mask = ~1;
                *field &= mask;
            }
            actor->word104 = 0;
        }
        Chase_SetState(actor, 3, 0, 0);
        actor->field2B8 = 0;
        if (ContactActor_IsControlled(actor) && ContactActor_ReadMotionFlags(actor, 0x2000)) {
            TileContact_EmitThreeParts((struct SceneActorRegistryActor *)actor);
        }
        actorFound = 1;
    }
    if (inRange[0] || inRange[1] || inRange[2] || inRange[3]) {
        if (gEngineFlags & 0x800) {
            if (!(self->triggerFlags & 0x10)) {
                self->state = 5;
                self->forced = 1;
            }
        } else {
            self->state = 7;
        }
        if (tetherInRange) {
            for (i = 0; i < 4; i++)
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 1))
                    self->triggerFlags |= 0x400;
            TetherActor_BeginDetachedState(1);
        } else {
            for (i = 0; i < 4; i++)
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 0))
                    self->triggerFlags |= 0x400;
        }
    } else if (tetherInRange) {
        ScriptTriggerActor_DispatchPendingScripts(self);
        TetherActor_BeginDetachedState(0);
        self->state = 5;
        self->forced = 1;
    } else if (actorFound && !(self->triggerFlags & 0x10)) {
        self->state = 5;
        self->forced = 1;
    }
}

static inline u8 TileTrigger_TestSlot(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

/* Takes the first free one of the pool's twelve slots: a cleared 0xC8-byte
   block recorded at +0x38 with its index at +0xBC; NULL when full. */

struct TileTrigger *Pool1B8_Allocate(struct TileTriggerManager *pool)
{
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!TileTrigger_TestSlot(&pool->used, i)) {
            struct TileTrigger *trigger = Heap_Alloc(sizeof(*trigger));
            Memory_Clear(trigger, sizeof(*trigger));
            pool->used |= 1 << i;
            trigger->index = i;
            pool->tiles[i] = trigger;
            return trigger;
        }
    }
    return 0;
}

u32 TetherActor_IsState(u32);

u8 TileTrigger_IsTetherNearby(struct TileTrigger *trigger)
{
    struct ContactPosition difference;
    struct ContactPosition *delta;
    struct SceneVec *position;
    struct ContactPosition *target;
    s32 x, z;
    if (gEngineFlags & 0x800)
        return 0;
    if (TetherActor_IsState(6))
        return 0;
    if (TetherActor_IsState(7))
        return 0;
    target = TetherActor_GetPosition();
    if (!target)
        return 0;
    delta = &difference;
    position = &trigger->position;
    delta->x = target->x - position->x;
    delta->y = target->y - position->y;
    delta->z = target->z - position->z;
    x = delta->x;
    if (x < 0)
        x = -x;
    if (x > 128)
        return 0;
    z = delta->z;
    if (z < 0)
        z = -z;
    if (z > 128)
        return 0;
    return 1;
}

/* Pool1B8_UpdateIdleState: Empty entry near the object state handlers; provisionally an idle state hook */

void Pool1B8_UpdateIdleState(void)
{
}

void TileTrigger_TickIdle(struct TileTrigger *trigger)
{
    struct SceneVec radius;
    u32 found;
    s32 i;
    trigger->sprite.flags |= 1;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 0; i <= 3; i++) {
        if (SceneActorQuery_FindPlayerInBounds(&trigger->position, &radius, i))
            goto activate;
    }
    if (TileTrigger_IsTetherNearby(trigger))
        goto activate;
    found = 0;
    if (sub_0806F06C((const struct ContactPosition *)&trigger->position,
                     (const struct ContactPosition *)&radius, &found)) {
    activate:
        trigger->state = 2;
        trigger->frame = 0;
    }
}

void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);

static inline u32 NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void TileTrigger_EmitParticles(struct TileTrigger *trigger)
{
    struct SceneVec position;
    u32 one;
    if (gEngineFlags & 0x800)
        return;
    switch (trigger->sequence) {
        case 0:
        case 1:
            one = 1;
            position = trigger->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            position.z = position.z + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = trigger->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            position.z = position.z - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = trigger->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            position.z = position.z + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = trigger->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            position.z = position.z - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 2, 0, 0, 32, 64, one, one,
                                                    512);
            position = trigger->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 2, 0, 0, 32, 64, one, one,
                                                    512);
            position = trigger->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            break;
    }
}

u32 Anim_SetSequence(void *, void *, u32, u32, u32);

void Sound_PlaySong(u32);

void TileTrigger_TickDelay(struct TileTrigger *trigger)
{
    struct TileTriggerManager *manager = gTileTriggerManager;
    if (trigger->frame >= trigger->delay) {
        TileTrigger_EmitParticles(trigger);
        Anim_SetSequence(&trigger->animation, manager->animationData, 0, (u8)trigger->sequence, 0);
        trigger->state = 4;
        trigger->frame = 0;
        switch (trigger->sequence) {
            case 0:
            case 1:
                if (gEngineFlags & 0x800)
                    Sound_PlaySong(0x26D);
                else
                    Sound_PlaySong(0x1AB);
                break;
        }
    } else
        trigger->frame++;
}

void Anim_RestartSequence(void *, void *, u32, u32, u32);

// confidence=low: plays the hold sequence while the 0x20 trigger flag is set; moves to state 6 when it ends.

void TileTrigger_StepHold(struct TileTrigger *trigger)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    manager = gTileTriggerManager;
    self = trigger;
    if (trigger->triggerFlags & 0x20) {
        trigger->sprite.flags &= ~1;
        {
            u8 *forced = &trigger->forced;
            if (*forced) {
                TileTrigger_EmitParticles(trigger);
                Anim_SetSequence(&trigger->animation, manager->animationData, 0,
                                 (u8)trigger->sequence, 0);
                *forced = 0;
                switch (trigger->sequence) {
                    case 0:
                    case 1:
                        if (gEngineFlags & 0x800)
                            Sound_PlaySong(0x26D);
                        else
                            Sound_PlaySong(0x1AB);
                        break;
                }
            }
        }
        if ((trigger->animation.index == 0 && trigger->animation.elapsed > 2) ||
            (trigger->animation.index == 1 && trigger->animation.elapsed != 0))
            TileTrigger_ApplyContactEffects(trigger);
        if (PlayerCue_StepSequenceEnded(&self->sprite, &trigger->animation)) {
            trigger->state = 6;
            trigger->triggerFlags = (trigger->triggerFlags & ~0x20) | 2;
        }
    }
}

void TileTrigger_StepPress(struct TileTrigger *trigger)
{
    struct ContactSwarmSprite *sprite = &trigger->sprite;
    sprite->flags &= ~1;
    if ((trigger->animation.index == 1) && (trigger->animation.elapsed != 0)) {
        TileTrigger_ApplyContactEffects(trigger);
        trigger->frame++;
    } else {
        PlayerCue_StepSequence(sprite, &trigger->animation);
    }
    if (trigger->frame > 30) {
        trigger->state = 5;
        trigger->forced = 1;
        trigger->frame = 0;
    }
}

// confidence=low: runs the trigger animation once the countdown passes 30 and advances the state when it ends.

void TileTrigger_StepActivation(struct TileTrigger *trigger)
{
    struct TileTrigger *self = trigger;
    struct TileTriggerManager *manager = gTileTriggerManager;
    if (trigger->timer > 30) {
        u8 *forced = &trigger->forced;
        if (*forced) {
            Anim_RestartSequence(&trigger->animation, manager->animationData, 0,
                                 (u8)trigger->sequence, 4);
            *forced = 0;
            switch (trigger->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (PlayerCue_StepSequenceEnded(&self->sprite, &trigger->animation)) {
            self->sprite.flags |= 1;
            trigger->state = 1;
            trigger->timer = 0;
            return;
        }
    } else if (trigger->animation.index == 1) {
        TileTrigger_ApplyContactEffects(trigger);
    }
    trigger->timer++;
}

void TileTrigger_StepRelease(struct TileTrigger *trigger)
{
    struct TileTrigger *self;
    struct TileTriggerManager *manager;
    self = trigger;
    manager = gTileTriggerManager;
    trigger->sprite.flags &= ~1;
    if (trigger->triggerFlags & 0x40) {
        u8 *forced = &trigger->forced;
        if (*forced) {
            Anim_RestartSequence(&trigger->animation, manager->animationData, 0,
                                 (u8)trigger->sequence, 4);
            *forced = 0;
            switch (trigger->sequence) {
                case 0:
                case 1:
                    Sound_PlaySong(0x1AC);
                    break;
            }
        }
        if (PlayerCue_StepSequenceEnded(&self->sprite, &trigger->animation)) {
            self->sprite.flags |= 1;
            trigger->state = 3;
            trigger->timer = 0;
            trigger->triggerFlags &= ~0x42;
            return;
        }
    } else {
        TileTrigger_ApplyContactEffects(trigger);
    }
    trigger->timer++;
}

void Pool1B8_RunPendingScripts(struct TileTrigger *trigger)
{
    ScriptTriggerActor_DispatchPendingScripts(trigger);
}

// confidence=low: restarts the trigger animation at frame 1, steps it once and enters state 6.

void TileTrigger_RestartHold(struct TileTriggerManager *manager, struct TileTrigger *trigger)
{
    trigger->sprite.flags &= ~1;
    Anim_SetSequence(&trigger->animation, manager->animationData, 0, (u8)trigger->sequence, 0);
    trigger->animation.index = 1;
    PlayerCue_StepSequenceEnded(&trigger->sprite, &trigger->animation);
    trigger->state = 6;
    trigger->triggerFlags = (trigger->triggerFlags & ~0x20) | 2;
}

void Pool1B8_EnterStateThree(struct TileTriggerManager *unused, struct TileTrigger *trigger)
{
    trigger->sprite.flags |= 1;
    trigger->state = 3;
    trigger->timer = 0;
    trigger->triggerFlags &= 0xFFBD;
}

void SceneStatusIndicator_Create(struct SceneVec *);

void SceneBody_Init(void *, s32, void *, u32, u32, void *);

void SceneBody_SetSprite(void *, void *);

void TerrainOverlay_QueueAddActor(void *);

void TileTrigger_CreateFromScript(void)
{
    struct TileTriggerManager *manager = gTileTriggerManager;
    struct TileTrigger *trigger;
    struct SceneVec pos;
    struct DisplaySequenceState *f;
    const vu16 *frame;
    struct ContactSwarmSprite *sprite;
    if (!manager) {
        manager = Pool1B8_Create();
        if (!manager)
            return;
    }
    trigger = Pool1B8_Allocate(manager);
    if (!trigger)
        return;
    trigger->state = 1;
    if (Script_SeekToKeyword('p')) {
        trigger->position.x = Script_GetValue();
        trigger->position.y = Script_GetValue();
        trigger->position.z = Script_GetValue();
    }
    trigger->key = Script_GetKeywordValue('i', -1);
    trigger->pendingScript = Script_GetKeywordValue('R', 0);
    if (Script_SeekToKeyword('G')) {
        s32 *dst = trigger->arguments;
        s32 i;
        for (i = 5; i >= 0; i--) {
            s32 v;
            if (Script_GetPc())
                v = Script_GetValue();
            else
                v = 0;
            *dst++ = v;
        }
    }
    trigger->oneShotScript = Script_GetKeywordValue('C', 0);
    if (Script_SeekToKeyword('A')) {
        s32 *dst = trigger->oneShotArguments;
        s32 i;
        for (i = 1; i >= 0; i--) {
            s32 v;
            if (Script_GetPc())
                v = Script_GetValue();
            else
                v = 0;
            *dst++ = v;
        }
    }
    {
        s32 v = Script_GetKeywordValue('T', 0);
        trigger->sequence = v;
        trigger->sequence = (u16)v > 4 ? 4 : v;
    }
    sprite = &trigger->sprite;
    trigger->sprite.position = *(struct ContactPosition *)&trigger->position;
    trigger->sprite.position.x -= 0x100;
    switch (trigger->sequence) {
        case 0:
            AnimData_SelectPaletteBank(&manager->graphics, 0x2EA);
            break;
        case 1:
            AnimData_SelectPaletteBank(&manager->graphics, 0x2EB);
            break;
    }
    Sprite_Init(sprite, &manager->graphics, 0);
    Anim_SetSequence(&trigger->animation, manager->animationData, 0, *(u8 *)&trigger->sequence, 0);
    f = &trigger->animation;
    frame = &f->commands[f->index];
    DisplaySequence_ApplyFrame((void *)sprite, (void *)f, (const u16 *)frame);
    if (++f->elapsed >= f->duration) {
        f->elapsed = 0;
        if (f->flags & 4) {
            u32 index = f->index;
            if (index == 0) {
                if (f->flags & 8) {
                    f->elapsed = f->duration;
                    goto next;
                }
                index = f->frameCount;
            }
            f->index = index - 1;
        } else if (++f->index >= f->frameCount) {
            if ((u8)(f->flags & 8)) {
                f->elapsed = f->duration;
                f->index = f->frameCount - 1;
            } else
                f->index = 0;
        }
    next:
        frame = &f->commands[f->index];
        f->frameWeight = *frame & 0xF;
        f->duration = (f->frameWeight * f->speed) >> 6;
        if (!f->duration)
            f->duration = 1;
    }
    sprite->flags |= 1;
    sprite->priority = 2;
    trigger->frame = 0;
    trigger->forced = 0;
    trigger->triggerFlags = 1;
    trigger->delay = Script_GetKeywordValue('o', 60);
    if (Script_GetKeywordValue('m', 0)) {
        trigger->triggerFlags |= 0x10;
        switch (Script_GetKeywordValue('S', 0)) {
            case 0:
                Pool1B8_EnterStateThree(manager, trigger);
                break;
            case 1:
                TileTrigger_RestartHold(manager, trigger);
                break;
        }
    }
    pos = trigger->position;
    SceneStatusIndicator_Create(&pos);
    if (Script_GetKeywordValue('s', 0)) {
        trigger->triggerFlags |= 8;
        SceneBody_Init(&trigger->body, trigger->key, &trigger->position, 0, 7, trigger);
        SceneBody_SetSprite(&trigger->body, trigger);
        TerrainOverlay_QueueAddActor(&trigger->body);
    }
}

/* The slot whose key (+0x8C, a name) matches, among the twelve used slots
   of the pool at 0x020001B8; NULL if none. */

struct TileTrigger *Pool1B8_FindByName(u32 key)
{
    struct TileTriggerManager *manager = gTileTriggerManager;
    struct TileTrigger **tiles = manager->tiles;
    s32 i;
    if (!manager->used)
        return 0;
    for (i = 0; i < 12; i++) {
        if (TileTrigger_TestSlot(&manager->used, i) && tiles[i]->key == key)
            return tiles[i];
    }
    return 0;
}

/* Provisional names and field interpretations; confidence low. */

void Tile_SetEnabledFromScript(void)
{
    s32 name = Script_GetKeywordValue(0x69, -1);
    s32 enabled;
    struct TileTrigger *tile;
    if (name >= 0) {
        enabled = Script_GetKeywordValue(0x73, 0);
        tile = Pool1B8_FindByName(name);
        if (tile) {
            if (enabled == 1) {
                tile->triggerFlags |= 0x200;
                tile->sprite.flags |= enabled;
            } else {
                tile->triggerFlags &= 0xFDFF;
                tile->state = 1;
            }
        }
    }
}

/* Script: 1 when the pool-1B8 slot named -i has both bits 0x180 of +0xC0
   set, else 0. */

u32 Pool1B8_IsReadyFromScript(void)
{
    s32 name = Script_GetKeywordValue('i', -1);
    struct TileTrigger *tile;
    if (name < 0 || (tile = Pool1B8_FindByName(name)) == 0 || (tile->triggerFlags & 0x180) != 0x180)
        return 0;
    return 1;
}

void TileTrigger_SetStateFromScript(void)
{
    s32 mode = Script_GetKeywordValue(0x73, 0);
    struct TileTrigger *trigger = Pool1B8_FindByName(Script_GetKeywordValue(0x69, 0));
    if (!trigger)
        return;
    switch (mode) {
        case 0:
            if (!(trigger->triggerFlags & 0x40)) {
                {
                    u32 flags = trigger->triggerFlags | 0x40;
                    u16 mask = 0xFFDF;
                    trigger->triggerFlags = flags & mask;
                }
                trigger->state = 6;
                trigger->forced = 1;
            }
            break;
        case 1:
            if (!(trigger->triggerFlags & 0x20)) {
                {
                    u32 flags = trigger->triggerFlags | 0x20;
                    u16 mask = 0xFFBF;
                    trigger->triggerFlags = flags & mask;
                }
                trigger->state = 3;
                trigger->forced = mode;
            }
            break;
    }
}

extern u32 gUnk_02000560;

extern void *gUnk_08606510[];

s32 Callback_InvokeActor(void *, void *);

static inline u8 Used(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}

static inline u32 TestPair(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

s32 TileTrigger_TickManager(struct TileTriggerManager *manager)
{
    struct TileTrigger **tiles = manager->tiles;
    s32 i;
    if (TestPair(0x400))
        return 0;
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    if (!manager->used)
        return 0;
    for (i = 0; i < 12; i++) {
        if (Used(&manager->used, i)) {
            struct TileTrigger *tile = tiles[i];
            if (!(tile->triggerFlags & 0x200))
                Callback_InvokeActor(tile, gUnk_08606510[tile->state]);
        }
    }
    return 0;
}

/* Provisional names and field interpretations; confidence low. */

void Heap_Free(void *);

void Sprite_Remove(void *);

void SceneBody_Unregister(void *);

static inline u8 TilePool_IsUsed(struct TileTriggerManager *pool, s32 i)
{
    if (pool->used & (1 << i))
        return 1;
    return 0;
}

u32 TilePool_Destroy(struct TileTriggerManager *pool)
{
    s32 i;
    struct TileTrigger *tile;
    for (i = 0; i < 12; i++) {
        if (TilePool_IsUsed(pool, i)) {
            tile = pool->tiles[i];
            Sprite_Remove(tile);
            if (tile->triggerFlags & 8) {
                TerrainOverlay_QueueRemoveActor(&tile->body);
                SceneBody_Unregister(&tile->body);
            }
            Heap_Free(tile);
        }
    }
    return (u32)(gTileTriggerManager = 0);
}

s32 Pool1B8_Init(struct TileTriggerManager *manager)
{
    AnimData_Load(&manager->graphics, 0xA945);
    manager->animationData = ScriptProc_Start(0x922E, 0xAE9);
    gTileTriggerManager = manager;
    return manager->used = 0;
}

struct TileTriggerManager *Pool1B8_Create(void)
{
    struct TileTriggerManager *manager;
    if (gTileTriggerManager != 0)
        return gTileTriggerManager;
    manager = Task_Create(8, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))TileTrigger_TickManager,
                          (void (*)(void *))TilePool_Destroy);
        if (Pool1B8_Init(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void Pool1B8_ResetInstance(void)
{
    gTileTriggerManager = 0;
}
