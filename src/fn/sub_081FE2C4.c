#include "global.h"
#include "engine.h"
#include "ram.h"

#include "sun_orb_effect.h"

extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void OrbitEffect_RemoveSlot(struct OrbitEffectManager *, struct OrbitEffect *, s32);
void OrbitEffect_UpdateTrailSprite(struct OrbitTrailSprite *, void *);
void OrbitEffect_SpawnOrbitTrail(struct OrbitEffect *);
void OrbitEffect_SpawnHomingTrail(struct OrbitEffect *, struct OrbitEffectPosition *);
void Collider_SetPosition(void *, void *, u32);
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
            struct ContactCollider *collider;
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
            if (effect->params.targetSource && !TestFlags(&effect->flags, 2)) {
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
