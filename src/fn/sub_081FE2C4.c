#include "global.h"
#include "engine.h"
#include "ram.h"

struct OrbitEffect;
struct OrbitEffectPosition {
    s16 x, y, z, pad;
};
struct OrbitEffectParams {
    u16 unk_00, lifetime;
    s16 kind;
    s8 angle;
    u8 unk_07;
    u32 movementEnabled;
    u8 unk_0C[8];
    s16 homing;
    u16 unk_16;
    s32 speed;
    s32 (*callback)(struct OrbitEffect *, s32);
};
struct OrbitEffectSprite {
    u32 flags;
    u8 unk_04[4];
    u8 scaleX, scaleY;
    u8 unk_0A[0x1C - 0x0A];
    struct OrbitEffectPosition position;
    u8 unk_24[8];
};
struct OrbitEffectCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[0x54 - 8];
};
struct OrbitTrailSprite {
    u32 flags;
    u8 unk_04[0x18 - 4];
    struct OrbitEffectPosition position;
    u8 unk_20[8];
    struct OrbitEffectPosition *target;
    u16 active, age;
    s16 angle, radius, velocityX, velocityZ;
};
struct OrbitEffect {
    u32 frame;
    u8 unk_04[8];
    struct OrbitEffectPosition velocity;
    struct OrbitEffectParams params;
    struct OrbitEffectSprite sprite, secondarySprite;
    u32 flags;
    u8 unk_90[0xA0 - 0x90];
    struct OrbitEffectCollider collider;
    u8 unk_F4[8];
    void *resource;
    struct OrbitTrailSprite trails[12];
    u16 nextTrail, trailTimer;
    s32 scale;
    u32 unk_3A8;
};
struct OrbitEffectTarget {
    u8 unk_00[0x260];
    struct OrbitEffectPosition position;
    u8 unk_268[0x5C2 - 0x268];
    s16 active;
};
struct OrbitEffectManager {
    u8 unk_00[0x18];
    u32 activeSlots;
    u8 unk_1C[0x38 - 0x1C];
    struct OrbitEffect slots[4];
    u32 unk_EE8, unk_EEC;
    s32 count;
    struct OrbitEffectTarget *target;
};
extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void OrbitEffect_RemoveSlot(struct OrbitEffectManager *, struct OrbitEffect *, s32);
void OrbitEffect_UpdateTrailSprite(struct OrbitTrailSprite *, void *);
void sub_081FDCB0(struct OrbitTrailSprite *, void *);
void OrbitEffect_SpawnOrbitTrail(struct OrbitEffect *);
void OrbitEffect_SpawnHomingTrail(struct OrbitEffect *, struct OrbitEffectPosition *);
void Collider_SetPosition(void *, void *, u32);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
void OrbitEffect_UpdateContraction(void);
void RisingActor_LowerHeight(void);
void OrbitEffect_HideTrailSprites(struct OrbitEffect *);

static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Polar(struct OrbitEffectPosition *out, struct OrbitEffectParams *params)
{
    s32 angle = params->angle;
    s32 speed = params->speed;
    out->x = Truncate(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Truncate(speed * gUnk_086149C4[angle & 255]);
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void AddPosition(struct OrbitEffectPosition *out, struct OrbitEffectPosition *in)
{
    out->x += in->x;
    out->y += in->y;
    out->z += in->z;
}
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline s32 ReadScale(struct OrbitEffect *effect)
{
    if (effect->scale >= 0)
        return effect->scale >> 12;
    return -((-effect->scale) >> 12);
}
// Updates the four managed orbit effects and their trail sprites.
s32 OrbitEffect_UpdateManager(struct OrbitEffectManager *manager)
{
    struct OrbitEffect *effect;
    s32 index;
    s32 j;
    for (effect = manager->slots, index = 0; index < 4; index++, effect++) {
        if (manager->activeSlots & (1 << index)) {
            struct OrbitEffectSprite *sprite = &effect->sprite;
            struct OrbitEffectParams *params = &effect->params;
            struct OrbitEffectCollider *collider;
            u32 mask = 0x400;
            if ((gEngineFlags | gScenePauseFlags) & mask)
                continue;
            {
                u32 pause = 4;
                if (gUnk_02000560 & pause)
                    continue;
            }
            if (effect->params.callback(effect, index) == 1) {
                OrbitEffect_RemoveSlot(manager, effect, index);
                continue;
            }
            if (effect->flags & 1) {
                Polar(&effect->velocity, params);
                ClearFlags(&effect->flags, ~1);
            }
            if (effect->params.movementEnabled && !TestFlags(&effect->flags, 2)) {
                AddPosition(&sprite->position, &effect->velocity);
                if (effect->params.homing == 0) {
                    if (sprite->position.x > 0xCDA)
                        sprite->position.x = 0xCDA;
                    else if (sprite->position.x <= 0x423)
                        sprite->position.x = 0x424;
                    if (sprite->position.z > 0xBD6)
                        sprite->position.z = 0xBD6;
                    else if (sprite->position.z <= 0x527)
                        sprite->position.z = 0x528;
                }
                effect->secondarySprite.position = effect->sprite.position;
            }
            if (effect->params.kind == 2 || effect->params.homing == 0) {
                for (j = 0; j < 12; j++) {
                    struct OrbitTrailSprite *trail = &effect->trails[j];
                    if (trail->active)
                        OrbitEffect_UpdateTrailSprite(trail, effect->resource);
                }
            } else {
                for (j = 0; j < 12; j++) {
                    struct OrbitTrailSprite *trail = &effect->trails[j];
                    if (trail->active)
                        sub_081FDCB0(trail, effect->resource);
                }
            }
            if (effect->params.kind == 2) {
                OrbitEffect_SpawnOrbitTrail(effect);
            } else if (effect->params.homing != 0) {
                struct OrbitEffectTarget *target = manager->target;
                if (target && target->active > 0) {
                    struct OrbitEffectPosition *origin = &target->position;
                    for (j = 0; j < 2; j++)
                        OrbitEffect_SpawnHomingTrail(effect, origin);
                }
            } else {
                OrbitEffect_SpawnOrbitTrail(effect);
            }
            effect->sprite.scaleX = ReadScale(effect);
            effect->sprite.scaleY = ReadScale(effect);
            effect->secondarySprite.scaleX = ReadScale(effect);
            effect->secondarySprite.scaleY = ReadScale(effect);
            collider = &effect->collider;
            Collider_SetPosition(collider, &effect->sprite.position, 0);
            if (effect->frame >= params->lifetime && params->lifetime != 0) {
                collider->flags |= 4;
                SunOrb_SetUpdateCallback(params, OrbitEffect_UpdateContraction);
            }
            if (effect->frame == params->lifetime - 150) {
                manager->count--;
                if (effect->params.kind == 2) {
                    SunOrb_SetUpdateCallback(params, RisingActor_LowerHeight);
                    OrbitEffect_HideTrailSprites(effect);
                }
            }
            ClearFlags(&effect->flags, ~2);
            effect->frame++;
        }
    }
    return 0;
}
