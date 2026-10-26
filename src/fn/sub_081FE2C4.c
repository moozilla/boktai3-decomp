#include "global.h"
#include "engine.h"
#include "ram.h"

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
    void *callback;
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
struct OrbitEffectMotion {
    struct OrbitEffectPosition velocity;
    struct OrbitEffectParams params;
    struct OrbitEffectSprite sprite, secondarySprite;
};
struct OrbitEffectState {
    u32 flags;
    u8 unk_04[0x70 - 4];
    void *resource;
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
s32 sub_08249240(void *, void *, void *);
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
    struct OrbitEffectPosition *output = out;
    s32 angle = params->angle;
    s32 speed = params->speed;
    output->x = Truncate(speed * gUnk_086149C4[(angle + 64) & 255]);
    output->y = 0;
    output->z = Truncate(speed * gUnk_086149C4[angle & 255]);
}
static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void AddPosition(struct OrbitEffectPosition *out, struct OrbitEffectPosition *in)
{
    u32 x = (u16)in->x;
    u32 currentX = (u16)out->x;
    x += currentX;
    out->x = x;
    {
        u32 y = (u16)in->y;
        out->y = y + (u16)out->y;
    }
    out->z = out->z + in->z;
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
#ifdef NONMATCHING
// fuzzy: 77.9%
// scope shape 64 (differences once low registers are set aside)
s32 OrbitEffect_UpdateManager(struct OrbitEffectManager *manager)
{
    struct OrbitEffect *effect = manager->slots;
    s32 index = 0;
    s32 j;
    struct OrbitEffectMotion *motion = (void *)&effect->velocity;
    struct OrbitEffectParams *params;
    struct OrbitEffectState *state = (void *)&effect->flags;
    do {
        if (manager->activeSlots & (1 << index)) {
            struct OrbitEffectSprite *sprite = &effect->sprite;
            u32 mask;
            params = &effect->params;
            mask = 0x400;
            if ((gEngineFlags | gScenePauseFlags) & mask)
                goto next;
            {
                u32 pause = 4;
                if (gUnk_02000560 & pause)
                    goto next;
            }
            if (sub_08249240(effect, (void *)index, motion->params.callback) == 1) {
                OrbitEffect_RemoveSlot(manager, effect, index);
                goto next;
            }
            {
                u32 moving = state->flags & 1;
                u32 *flags = &state->flags;
                if (moving) {
                    Polar(&motion->velocity, params);
                    ClearFlags(flags, ~1);
                }
                if (motion->params.movementEnabled && !TestFlags(flags, 2)) {
                    AddPosition(&sprite->position, &motion->velocity);
                    if (motion->params.homing == 0) {
                        if (sprite->position.x > 0xCDA)
                            sprite->position.x = 0xCDA;
                        else if (sprite->position.x <= 0x423)
                            sprite->position.x = 0x424;
                        if (sprite->position.z > 0xBD6)
                            sprite->position.z = 0xBD6;
                        else if (sprite->position.z <= 0x527)
                            sprite->position.z = 0x528;
                    }
                    motion->secondarySprite.position = motion->sprite.position;
                }
                if (motion->params.kind == 2 || motion->params.homing == 0) {
                    for (j = 0; j < 12; j++) {
                        struct OrbitTrailSprite *trail = &effect->trails[j];
                        if (trail->active)
                            OrbitEffect_UpdateTrailSprite(trail, state->resource);
                    }
                } else {
                    for (j = 0; j < 12; j++) {
                        struct OrbitTrailSprite *trail = &effect->trails[j];
                        if (trail->active)
                            sub_081FDCB0(trail, state->resource);
                    }
                }
                if (motion->params.kind == 2) {
                    OrbitEffect_SpawnOrbitTrail(effect);
                } else if (motion->params.homing != 0) {
                    struct OrbitEffectTarget *target = manager->target;
                    if (target && target->active > 0) {
                        struct OrbitEffectPosition *origin = &target->position;
                        j = 1;
                        do {
                            OrbitEffect_SpawnHomingTrail(effect, origin);
                        } while (--j >= 0);
                    }
                } else {
                    OrbitEffect_SpawnOrbitTrail(effect);
                }
                effect->sprite.scaleX = ReadScale(effect);
                effect->sprite.scaleY = ReadScale(effect);
                effect->secondarySprite.scaleX = ReadScale(effect);
                effect->secondarySprite.scaleY = ReadScale(effect);
                {
                    struct OrbitEffectCollider *collider = &effect->collider;
                    struct OrbitEffectPosition *position = &sprite->position;
                    Collider_SetPosition(collider, position, 0);
                    if (effect->frame >= params->lifetime && params->lifetime != 0) {
                        collider->flags |= 4;
                        SunOrb_SetUpdateCallback(params, OrbitEffect_UpdateContraction);
                    }
                    if (effect->frame == params->lifetime - 150) {
                        manager->count--;
                        if (motion->params.kind == 2) {
                            SunOrb_SetUpdateCallback(params, RisingActor_LowerHeight);
                            OrbitEffect_HideTrailSprites(effect);
                        }
                    }
                    ClearFlags(flags, ~2);
                    effect->frame++;
                }
            }
        }
    next:
        index++;
        motion = (void *)((u8 *)motion + sizeof(*effect));
        state = (void *)((u8 *)state + sizeof(*effect));
        effect++;
    } while (index <= 3);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", OrbitEffect_UpdateManager);
#endif
