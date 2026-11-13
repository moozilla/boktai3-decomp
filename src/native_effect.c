#include "native_effect.h"
#include "global.h"
#include "ram.h"
#include "scene_callback_pool.h"
#include "actor_attachment_state.h"
#include "background_controller.h"
#include "player_cue_sequence.h"

u32 ActorEffect_SpawnAtActorPos(struct ScenePoolActor *actor, struct ContactPosition *offset,
                                const struct ContactPosition *velocity, u32 resourceId,
                                u32 sequenceResource, u32 sequence, u32 keyframe, u32 row,
                                u32 variant, u32 palette, u32 timer, u32 motion, u32 visualFlags,
                                u32 priority, u32 restitution)
{
    struct ContactPosition projected;
    struct ContactPosition *position = &projected;
    position->x = actor->position.x >> 12;
    position->y =
        ((actor->position.z + (actor->position.y + actor->restrictionMetric)) >> 12) + 0x60;
    position->z = actor->position.z >> 12;
    if (offset != NULL) {
        position->x += offset->x;
        position->y += offset->y;
        position->z += offset->z;
    }
    return sub_08113398(position, velocity, resourceId, sequenceResource, sequence, keyframe, row,
                        variant, palette, timer, motion, visualFlags, priority, restitution);
}

u32 ActorParticles_CreateAtOffset(struct ScenePoolActor *actor, struct ContactPosition *offset,
                                  const struct ContactPosition *velocity, u32 resourceId,
                                  u32 sequenceResource, u32 sequence, u32 keyframe, u32 row,
                                  u32 variant, u32 palette, u32 timer, u32 motion, u32 visualFlags,
                                  u32 priority, u32 restitution)
{
    struct ContactPosition projected;
    struct ContactPosition *position = &projected;
    position->x = actor->position.x >> 12;
    position->y =
        ((actor->position.z + (actor->position.y + actor->restrictionMetric)) >> 12) + 0x60;
    position->z = actor->position.z >> 12;
    if (offset != NULL) {
        position->x += offset->x;
        position->y += offset->y;
        position->z += offset->z;
    }
    return sub_08113398(position, velocity, resourceId, sequenceResource, sequence, keyframe, row,
                        variant, palette, timer, motion, visualFlags, priority, restitution);
}

u32 SpriteEffect_CreateAtPosition(struct NativeEffectVector *actor, struct ContactPosition *offset,
                                  const struct ContactPosition *velocity, u32 resourceId,
                                  u32 sequenceResource, u32 sequence, u32 keyframe, u32 row,
                                  u32 variant, u32 palette, u32 timer, u32 motion, u32 visualFlags,
                                  u32 priority, u32 restitution)
{
    struct ContactPosition projected;
    struct ContactPosition *position = &projected;
    position->x = actor->x >> 12;
    position->y = ((actor->z + actor->y) >> 12) + 0x60;
    position->z = actor->z >> 12;
    if (offset != NULL) {
        position->x += offset->x;
        position->y += offset->y;
        position->z += offset->z;
    }
    return sub_08113398(position, velocity, resourceId, sequenceResource, sequence, keyframe, row,
                        variant, palette, timer, motion, visualFlags, priority, restitution);
}

/* SceneDebrisPool_ResetSingleton: Clears the debris pool singleton pointer */

void SceneDebrisPool_ResetSingleton(void)
{
    gDebrisPool = 0;
}

s32 Terrain_GetHeightAtFixedPosition(void *);

s32 SceneGroundEffect_IsBelowGround(struct ScenePoolSlot *effect, s32 offsetY)
{
    struct NativeEffectVector *position = &effect->position;
    s32 terrainHeight = Terrain_GetHeightAtFixedPosition(position);
    s32 height = effect->height;
    s32 heightAdjustedY = position->y;
    s32 projectedY;
    heightAdjustedY += height;
    projectedY = position->z;
    projectedY += heightAdjustedY;
    projectedY >>= 12;
    projectedY += 0x60;
    projectedY += offsetY;
    if (terrainHeight <= projectedY)
        return 1;
    return 0;
}

void NativeEffect_SetState(struct ScenePoolSlot *slot,
                           void (*callback)(struct ScenePoolSlot *, u32))
{
    slot->callback = callback;
    slot->key.entered = 1;
}

void Sprite_Remove(void *);

void NativeEffect_DestroyEntry(struct ScenePoolSlot *effect)
{
    u8 *entered = &effect->key.entered;
    if (*entered != 0) {
        u8 *state;
        u32 zero;
        Sprite_Remove(&effect->sprite);
        state = &effect->key.active;
        zero = 0;
        *state = zero;
        state = &effect->state;
        *state = zero;
        effect->callback = (void *)zero;
        *entered = zero;
    }
}

void Sound_PlaySong(u32);

void SpriteEffect_RemoveWithBurst(struct ScenePoolSlot *actor)
{
    struct NativeEffectPackedBits offset;
    if (actor->key.entered) {
        Sprite_Remove(&actor->sprite);
        actor->key.active = 0;
        actor->state = 0;
        actor->callback = 0;
        actor->key.entered = 0;
        offset.x = 0;
        offset.y = actor->height >> 12;
        offset.z = 0;
        SpriteEffect_CreateAtPosition(&actor->position, (void *)&offset, 0, 0x8639, 0x75f6, 0, 0, 0,
                                      0, 0x153, 30, 0, 0, 1, 0);
        Sound_PlaySong(0x16e);
    }
}

extern const s16 gUnk_086149C4[];

extern const u16 gUnk_086141BC[];

s32 Mod(s32, s32);

void NativeParticle_CreateWithFlag(void *, s32, void *, s32, s32, s32, s32);

void Collider_SetFixedPositionWithYOffset(void *, struct NativeEffectVector *, s32);

void SpatialIndex_InsertEntry(void *);

static inline s32 NativeEffect_ScaleFixed(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline void NativeEffect_GetDirection(struct NativeEffectPacked *out, u8 angle, s32 speed)
{
    out->x = NativeEffect_ScaleFixed(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = NativeEffect_ScaleFixed(gUnk_086149C4[angle] * speed);
}

static inline void NativeEffect_Project(struct NativeEffectPacked *out,
                                        struct NativeEffectVector *pos, s32 h)
{
    out->x = (pos->x >> 12) - gSceneCamera->x;
    out->y = ((s32)(pos->z + (pos->y + h)) >> 12) + 0x60;
    out->z = pos->z >> 12;
}

// confidence=low: type zero and threshold sprites travel at fixed velocity.

void NativeEffect_UpdateLinearFlight(struct ScenePoolSlot *effect)
{
    struct NativeEffectPacked projected;
    if (effect->key.entered) {
        effect->sprite.flags &= ~1;
        effect->key.entered = 0;
    }
    effect->position.x += effect->velocity.x;
    effect->position.y += effect->velocity.y;
    effect->position.z += effect->velocity.z;
    NativeEffect_Project(&projected, &effect->position, effect->height);
    *(struct NativeEffectPacked *)&effect->sprite.position = projected;
    PlayerCue_StepSequenceEnded(&effect->sprite, &effect->animation);
    Collider_SetFixedPositionWithYOffset(&effect->collider, &effect->position, effect->height);
    SpatialIndex_InsertEntry(&effect->collider);
    if ((u8)SceneGroundEffect_IsBelowGround(effect, 8)) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))NativeEffect_DestroyEntry);
    } else if (!--effect->lifetime) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))NativeEffect_DestroyEntry);
    }
}

// confidence=low: profile-driven native effect motion and creation.

static inline u32 NativeEffect_GetTrailRandom(void)
{
    u32 value;
    typeof(value) t1;
    value = (t1 = gUnk_086141BC[gEffectRandomTableIndex]);
    value &= 7;
    return value;
}

static inline u8 Odd(u32 t)
{
    return t & 1;
}

// confidence=low: horizontal motion accelerates and leaves a short particle trail.

inline static u8 NativeEffect_IsOddFrame(u32 t)
{
    {
        u32 k1 = 1;
        return t & k1;
    }
}

void NativeEffect_UpdateAcceleratingTrail(struct ScenePoolSlot *effect)
{
    struct NativeEffectPacked projected;
    if (effect->key.entered) {
        effect->sprite.flags &= ~1;
        effect->timer = 0;
        effect->key.entered = 0;
    }
    effect->position.x += effect->velocity.x;
    effect->position.y += effect->velocity.y;
    effect->position.z += effect->velocity.z;
    NativeEffect_Project(&projected, &effect->position, effect->height);
    *((struct NativeEffectPacked *)(&effect->sprite.position)) = projected;
    PlayerCue_StepSequenceEnded(&effect->sprite, &effect->animation);
    Collider_SetFixedPositionWithYOffset(&effect->collider, &effect->position, effect->height);
    SpatialIndex_InsertEntry(&effect->collider);
    effect->velocity.x += effect->direction << 9;
    if ((effect->timer <= 9) && NativeEffect_IsOddFrame(effect->timer)) {
        struct {
            u32 x : 16;
            u32 y : 16;
            u32 z : 16;
            u32 pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = NativeEffect_GetTrailRandom() - 3;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = NativeEffect_GetTrailRandom() - 3;
        NativeParticle_CreateWithFlag(&effect->sprite.position, 0, &offset, 1, 1, 0x200, 0);
    }
    effect->timer++;
    if ((u8)SceneGroundEffect_IsBelowGround(effect, 0)) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))SpriteEffect_RemoveWithBurst);
    } else if (!(--effect->lifetime)) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))SpriteEffect_RemoveWithBurst);
    }
}

// confidence=low: profile-driven native effect motion and creation.

// confidence=low: type one follows a widening rotating trajectory.

void NativeEffect_UpdateRotatingFlight(struct ScenePoolSlot *effect)
{
    struct NativeEffectPacked velocity, projected;
    if (effect->key.entered) {
        effect->sprite.flags &= ~1;
        effect->timer = 0;
        effect->key.entered = 0;
    }
    effect->position.x += effect->velocity.x;
    effect->position.y += effect->velocity.y;
    effect->position.z += effect->velocity.z;
    if (effect->timer <= 9) {
        NativeEffect_GetDirection(&velocity, effect->angle, 3);
        if (!Mod(effect->timer, 4))
            effect->angle += effect->direction * 3;
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    } else if (effect->timer <= 19) {
        NativeEffect_GetDirection(&velocity, effect->angle, 3);
        if (!Mod(effect->timer, 2))
            effect->angle += effect->direction * 16;
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    } else {
        NativeEffect_GetDirection(&velocity, effect->angle, 4);
        if (effect->timer <= 41)
            effect->angle += effect->direction * 4;
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    }
    effect->timer++;
    NativeEffect_Project(&projected, &effect->position, effect->height);
    *(struct NativeEffectPacked *)&effect->sprite.position = projected;
    PlayerCue_StepSequenceEnded(&effect->sprite, &effect->animation);
    {
        void *c = &effect->collider;
        s32 *hp = &effect->height;
        s32 h;
        h = *hp;
        Collider_SetFixedPositionWithYOffset(c, &effect->position, h);
    }
    SpatialIndex_InsertEntry(&effect->collider);
    if ((u8)SceneGroundEffect_IsBelowGround(effect, 8)) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))NativeEffect_DestroyEntry);
    } else if (!--effect->lifetime) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))NativeEffect_DestroyEntry);
    }
}

static inline void NativeEffect_GetWideDirection(struct NativeEffectPacked *out, u16 angle,
                                                 s32 speed)
{
    s16 a = angle;
    out->x = NativeEffect_ScaleFixed(gUnk_086149C4[(a + 64) & 255] * speed);
    out->y = 0;
    out->z = NativeEffect_ScaleFixed(gUnk_086149C4[angle] * speed);
}

static inline u32 NativeEffect_GetRandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}

// confidence=low: type three moves along a changing heading and emits trail particles.

void NativeEffect_UpdateTurningTrail(struct ScenePoolSlot *effect)
{
    struct NativeEffectPacked velocity, projected;
    if (effect->key.entered) {
        effect->sprite.flags &= ~1;
        effect->timer = 0;
        effect->key.entered = 0;
    }
    effect->position.x += effect->velocity.x;
    effect->position.y += effect->velocity.y;
    effect->position.z += effect->velocity.z;
    if (effect->timer <= 5) {
        NativeEffect_GetWideDirection(&velocity, effect->angle, 2);
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    } else if (effect->timer <= 19) {
        effect->angle += effect->direction * 2;
        if (effect->timer == 19)
            effect->angle = 0;
        NativeEffect_GetWideDirection(&velocity, effect->angle, 1);
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    } else {
        NativeEffect_GetWideDirection(&velocity, effect->angle, 3);
        if (!Mod(effect->timer, 4))
            effect->speed += 0x1000;
        effect->velocity.x = (velocity.x << 12) + effect->speed;
        effect->velocity.z = velocity.z << 12;
    }
    NativeEffect_Project(&projected, &effect->position, effect->height);
    *(struct NativeEffectPacked *)&effect->sprite.position = projected;
    if (effect->timer <= 19 && (effect->timer & 1)) {
        struct {
            u32 x : 16, y : 16, z : 16, pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = (NativeEffect_GetRandomValue() & 15) - 7;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = (NativeEffect_GetRandomValue() & 15) - 7;
        NativeParticle_CreateWithFlag(&effect->sprite.position, 0, &offset, 1, 1, 0x200, 0);
    }
    effect->timer++;
    PlayerCue_StepSequenceEnded(&effect->sprite, &effect->animation);
    Collider_SetFixedPositionWithYOffset(&effect->collider, &effect->position, effect->height);
    SpatialIndex_InsertEntry(&effect->collider);
    if ((u8)SceneGroundEffect_IsBelowGround(effect, 0)) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))SpriteEffect_RemoveWithBurst);
    } else if (!--effect->lifetime) {
        NativeEffect_SetState(effect,
                              (void (*)(struct ScenePoolSlot *, u32))SpriteEffect_RemoveWithBurst);
    }
}

s32 SceneCallbackPool_Update(struct SceneCallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (pool->slots[i].key.active && pool->slots[i].callback)
            pool->slots[i].callback(&pool->slots[i], i * sizeof(pool->slots[i]));
    }
    pool->frame++;
    return 0;
}

s32 NativeEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 0xf; i++)
        SlotRegistry_RemoveSprite(i);
    gSceneCallbackPool = 0;
    return 0;
}

s32 SceneCallbackPool_Initialize(struct SceneCallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        Memory_Clear(&pool->slots[i].sprite.position, 8);
        pool->slots[i].key.active = 0;
    }
    pool->frame = 0;
    gSceneCallbackPool = pool;
    return 0;
}

struct SceneCallbackPool *NativeEffect_EnsureManager(void)
{
    struct SceneCallbackPool *p;
    if (gSceneCallbackPool != 0)
        return gSceneCallbackPool;
    p = Task_Create(0xa, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))SceneCallbackPool_Update,
                          (void (*)(void *))NativeEffect_DestroyManager);
        if (SceneCallbackPool_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return (struct SceneCallbackPool *)p;
}

s32 SlotRegistry_ClaimMatchingSlot(struct SceneCallbackPool *registry, s32 start, s32 id,
                                   u8 *claimed)
{
    struct NativeEffectSlotKey *slot;
    s32 index = start;
    u8 *entry;
    u32 offset;
    if (index <= 15) {
        offset = index * sizeof(struct ScenePoolSlot);
        slot = (struct NativeEffectSlotKey *)(offset + (u32)&registry->slots[0].key);
        do {
            if (slot->active) {
                entry = claimed + index;
                if (!*entry && slot->id == id) {
                    *entry = 1;
                    return 1;
                }
            }
            slot = (void *)((u8 *)slot + sizeof(struct ScenePoolSlot));
            index++;
        } while (index <= 15);
    }
    return 0;
}

s32 EffectPool_CheckCapacity(struct SceneCallbackPool *pool, s32 group, s32 required,
                             s32 allowGroups)
{
    u8 visited[16];
    s32 groups = 0;
    s32 freeSlots = 0;
    s32 i;
    u32 g = group & 0xff;
    Memory_Clear(visited, 16);
    for (i = 0; i < 16; i++) {
        if (!pool->slots[i].key.active)
            freeSlots++;
        else if ((u8)pool->slots[i].key.id == g && !visited[i]) {
            visited[i] = 1;
            SlotRegistry_ClaimMatchingSlot(pool, i + 1, pool->slots[i].key.id, visited);
            groups++;
        }
    }
    if (!allowGroups && groups > 2)
        return 1;
    if (freeSlots < required)
        return 1;
    return 0;
}

void NativeEffect_OnColliderExpire(struct ScenePoolSlot *p)
{
    NativeEffect_SetState(p, (void (*)(struct ScenePoolSlot *, u32))NativeEffect_DestroyEntry);
}

void NativeEffect_OnColliderImpact(struct ScenePoolSlot *p)
{
    NativeEffect_SetState(p, (void (*)(struct ScenePoolSlot *, u32))SpriteEffect_RemoveWithBurst);
}

/* Threshold profiles use this callback to continue after contact. */

void NativeEffect_OnColliderIgnore(void)
{
}

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Z {
    struct H a, b;
};

void Position_GetHeightAdjustedCoordinates(void *, u32, void *);

void SpatialIndex_InitEntry(void *, void *, struct Z *, struct Z *, u32, u32, u32, u32, u32, u32,
                            u32);

void NativeEffect_InitDefaultCollider(struct ScenePoolSlot *effect, void (*callback)(void),
                                      u32 damage)
{
    u32 position[2];
    struct Z bounds, offset;
    Position_GetHeightAdjustedCoordinates(&effect->position, effect->height, position);
    bounds.a.lo = 0xe;
    bounds.a.hi = 0xe;
    bounds.b.lo = 8;
    *(u32 *)&offset.a = 0xFFFC0000;
    *(u32 *)&offset.b &= 0xFFFF0000;
    SpatialIndex_InitEntry(&effect->collider, position, &bounds, &offset, 0xc, (u8)effect->key.id,
                           5, damage, 0xc, callback, (u32)effect);
}

void NativeEffect_InitColliderWithDepth(struct ScenePoolSlot *effect, void (*callback)(void),
                                        u32 damage, u32 depth)
{
    u32 position[2];
    struct Z bounds, offset;
    Position_GetHeightAdjustedCoordinates(&effect->position, effect->height, position);
    bounds.a.lo = 0xe;
    bounds.a.hi = 0xe;
    bounds.b.lo = depth;
    *(u32 *)&offset.a = 0xFFFC0000;
    *(u32 *)&offset.b &= 0xFFFF0000;
    SpatialIndex_InitEntry(&effect->collider, position, &bounds, &offset, 0xc, (u8)effect->key.id,
                           5, damage, 0xc, callback, (u32)effect);
}

void NativeEffect_InitTypeZeroSprite(struct ScenePoolSlot *effect)
{
    struct SceneAnimation *graphics = &effect->graphics;
    if (AnimData_Load(graphics, 0x409C) != 0) {
        void *sequence;
        Sprite_Init(&effect->sprite, graphics, 0);
        sequence = ScriptProc_Start(0x922E, 0x1C22);
        effect->data = sequence;
        Anim_SetSequence(&effect->animation, sequence, 5, 0, 0);
        AnimData_SelectPaletteBank(graphics, 0xe4);
    }
}

void NativeEffect_InitThresholdSprite(struct ScenePoolSlot *effect, s32 threshold)
{
    struct SceneAnimation *graphics = &effect->graphics;
    if (AnimData_Load(graphics, 0x409c) != 0) {
        void *sequence;
        void **sequenceSlot;
        Sprite_Init(&effect->sprite, graphics, 0);
        sequence = ScriptProc_Start(0x922e, 0x1c22);
        sequenceSlot = &effect->data;
        *sequenceSlot = (void *)sequence;
        if (threshold > 0x3c) {
            Anim_SetSequence(&effect->animation, sequence, 5, 2, 0);
        } else {
            Anim_SetSequence(&effect->animation, sequence, 5, 0, 0);
        }
        AnimData_SelectPaletteBank(&effect->graphics, 0xe4);
    }
}

void NativeEffect_InitDirectionalSprite(struct ScenePoolSlot *effect, s32 reverse)
{
    struct SceneAnimation *graphics = &effect->graphics;
    if (AnimData_Load(graphics, 0x409c) != 0) {
        void *sequence;
        void **sequenceSlot;
        Sprite_Init(&effect->sprite, graphics, 0);
        sequence = ScriptProc_Start(0x922e, 0x1c22);
        sequenceSlot = &effect->data;
        *sequenceSlot = (void *)sequence;
        Anim_SetSequence(&effect->animation, sequence, 5, 1, 0);
        AnimData_SelectPaletteBank(graphics, 0xe4);
        if (reverse == 0) {
            effect->angle = 0xe0;
            effect->direction = 1;
        } else {
            u8 *angle = &effect->angle;
            s32 initialAngle = 0x20;
            *angle = initialAngle;
            effect->direction = initialAngle - 0x21;
        }
    }
}

void NativeEffect_InitUnsequencedSprite(struct ScenePoolSlot *effect, s32 unused)
{
    struct SceneAnimation *graphics = &effect->graphics;
    if (AnimData_Load(graphics, 0x409c) != 0) {
        Sprite_Init(&effect->sprite, graphics, 0);
        effect->data = ScriptProc_Start(0x922e, 0x1c22);
        AnimData_SelectPaletteBank(graphics, 0xe4);
    }
}

void EffectActor_BeginHorizontalMotion(struct ScenePoolSlot *actor,
                                       struct NativeEffectProfile *config, s32 reverse)
{
    actor->velocity.x += 0x2000;
    actor->speed = actor->velocity.x;
    NativeEffect_InitColliderWithDepth(actor, NativeEffect_OnColliderImpact, config->damage, 10);
    NativeEffect_SetState(
        actor, (void (*)(struct ScenePoolSlot *, u32))NativeEffect_UpdateAcceleratingTrail);
    if (reverse) {
        actor->position.x -= 0x10000;
        Anim_SetSequence(&actor->animation, actor->data, 8, 0, 1);
        {
            s32 velocity = actor->velocity.x;
            actor->velocity.x = actor->speed - velocity;
        }
        actor->direction = -1;
    } else {
        actor->position.x += 0x8000;
        Anim_SetSequence(&actor->animation, actor->data, 8, 0, 0);
        actor->direction = 1;
    }
}

void NativeEffect_InitDirectionalTypeThreeSprite(struct ScenePoolSlot *effect, s32 reverse)
{
    struct SceneAnimation *graphics = &effect->graphics;
    if (AnimData_Load(graphics, 0x409c) != 0) {
        void *sequence;
        void **sequenceSlot;
        Sprite_Init(&effect->sprite, graphics, 0);
        sequence = ScriptProc_Start(0x922e, 0x1c22);
        sequenceSlot = &effect->data;
        *sequenceSlot = (void *)sequence;
        Anim_SetSequence(&effect->animation, sequence, 8, 0, 0);
        AnimData_SelectPaletteBank(graphics, 0xe4);
        if (reverse == 0) {
            effect->angle = 0xc0;
            effect->direction = 1;
        } else {
            u8 *angle = &effect->angle;
            s32 initialAngle = 0x60;
            *angle = initialAngle;
            effect->direction = initialAngle - 0x61;
        }
    }
}

struct SceneCallbackPool *NativeEffect_EnsureManager(void);

void NativeEffect_OnColliderIgnore(void);

void NativeEffect_UpdateLinearFlight(struct ScenePoolSlot *);

void NativeEffect_UpdateRotatingFlight(struct ScenePoolSlot *);

void NativeEffect_UpdateTurningTrail(struct ScenePoolSlot *);

static inline u32 NativeEffect_GroupId(u32 frame, s32 group)
{
    u32 mask = 255;
    return (group & mask) | ((frame & mask) << 8);
}

static inline s32 NativeEffect_ThresholdDamage(struct NativeEffectProfile *profile)
{
    if (profile->threshold > 60)
        return profile->damage * 2;
    return profile->damage;
}

/* Select a sprite and motion state from the emission profile. */

s32 NativeEffect_CreateFromProfile(struct NativeEffectVector *position, s32 height,
                                   struct NativeEffectVector *velocity,
                                   struct NativeEffectProfile *profile, s32 group, s32 reverse,
                                   s32 required, s32 allowGroups)
{
    struct SceneCallbackPool *pool;
    s32 damage;
    s32 i;
    if (!gSceneCallbackPool) {
        pool = NativeEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gSceneCallbackPool;
    if ((u8)EffectPool_CheckCapacity(pool, NativeEffect_GroupId(pool->frame, group), required,
                                     allowGroups))
        return -1;
    for (i = 0; i < 16; i++) {
        if (!pool->slots[i].key.active) {
            switch (profile->type) {
                case 0:
                    NativeEffect_InitTypeZeroSprite(&pool->slots[i]);
                    break;

                case 1:
                    NativeEffect_InitDirectionalSprite(&pool->slots[i], reverse);
                    break;

                case 2:
                    NativeEffect_InitThresholdSprite(&pool->slots[i], profile->threshold);
                    break;

                case 3:
                    NativeEffect_InitDirectionalTypeThreeSprite(&pool->slots[i], reverse);
                    break;

                case 4:
                    NativeEffect_InitUnsequencedSprite(&pool->slots[i], reverse);
                    break;
            }

            pool->slots[i].position = *position;
            pool->slots[i].velocity = *velocity;
            pool->slots[i].height = height;
            pool->slots[i].lifetime = profile->lifetime;
            pool->slots[i].sprite.flags |= 0x11;
            pool->slots[i].sprite.priority = 1;
            pool->slots[i].key.active = 1;
            pool->slots[i].state = 0;
            group = NativeEffect_GroupId(pool->frame, group);
            pool->slots[i].key.id = group;
            switch (profile->type) {
                case 0:
                    pool->slots[i].position.x += 0x4000;
                    pool->slots[i].velocity.x += 0x4000;
                    pool->slots[i].speed = pool->slots[i].velocity.x >> 12;
                    NativeEffect_InitDefaultCollider(&pool->slots[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(
                        &pool->slots[i],
                        (void (*)(struct ScenePoolSlot *, u32))NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 2:
                    pool->slots[i].position.x += 0xc000;
                    pool->slots[i].velocity.x += 0x5000;
                    pool->slots[i].speed = pool->slots[i].velocity.x >> 12;
                    if (profile->threshold > 60) {
                        damage = NativeEffect_ThresholdDamage(profile);
                        NativeEffect_InitDefaultCollider(&pool->slots[i],
                                                         NativeEffect_OnColliderIgnore, damage);
                    } else {
                        damage = NativeEffect_ThresholdDamage(profile);
                        NativeEffect_InitDefaultCollider(&pool->slots[i],
                                                         NativeEffect_OnColliderExpire, damage);
                    }
                    NativeEffect_SetState(
                        &pool->slots[i],
                        (void (*)(struct ScenePoolSlot *, u32))NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 1:
                    pool->slots[i].position.x += 0x4000;
                    pool->slots[i].velocity.x += 0x800;
                    pool->slots[i].speed = pool->slots[i].velocity.x;
                    NativeEffect_InitDefaultCollider(&pool->slots[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(
                        &pool->slots[i],
                        (void (*)(struct ScenePoolSlot *, u32))NativeEffect_UpdateRotatingFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 3:
                    pool->slots[i].position.x += 0x10000;
                    pool->slots[i].velocity.x += 0x2000;
                    pool->slots[i].speed = pool->slots[i].velocity.x;
                    NativeEffect_InitColliderWithDepth(
                        &pool->slots[i], NativeEffect_OnColliderImpact, profile->damage, 10);
                    NativeEffect_SetState(
                        &pool->slots[i],
                        (void (*)(struct ScenePoolSlot *, u32))NativeEffect_UpdateTurningTrail);
                    Sound_PlaySong(0x164);
                    break;

                case 4:
                    EffectActor_BeginHorizontalMotion(&pool->slots[i], profile, reverse);
                    Sound_PlaySong(0x164);
                    break;
            }

            return i;
        }
    }

    return -1;
}

void SlotRegistry_RemoveSprite(u32 index)
{
    struct SceneCallbackPool *registry = gSceneCallbackPool;
    if (registry && index <= 15) {
        if (registry->slots[index].key.active && registry->slots[index].sprite.unknown4[0]) {
            Sprite_Remove(&registry->slots[index].sprite);
            registry->slots[index].key.active = 0;
            registry->slots[index].key.id = 0;
            registry->slots[index].callback = 0;
        }
    }
}
