#include "global.h"
#include "contact_actor.h"
#include "engine.h"
#include "ram.h"

struct ScheduledEffectBody {
    u32 id, flags, unk_08;
    struct ContactPosition position;
    u8 unk_14[0x34];
};
struct ScheduledEffectSprite {
    u32 flags;
    u8 unk_04[0x28];
};
struct SceneScheduledEffect {
    u8 unk_00[0x60];
    struct ScheduledEffectBody body;
    struct ScheduledEffectSprite sprite;
    struct ContactPosition position;
    u8 collider[0x54];
    u16 variant, animationIndex, active;
    s16 lifetime, delay, state, timer, slot;
    u16 flags;
    u8 unk_142[2];
};
struct ColliderVector {
    s16 x, y, z;
};
extern const struct ColliderVector gUnk_08610710, gUnk_08610718, gUnk_08610720;
extern u32 gUnk_02000560;
struct SceneScheduledEffect *sub_0812298C(void *);
void SceneBody_Init(struct ScheduledEffectBody *, u32, struct ContactPosition *, u32, u32, void *);
void SceneBody_SetSprite(struct ScheduledEffectBody *, struct ScheduledEffectSprite *);
void TerrainOverlay_QueueAddActor(struct ScheduledEffectBody *);
void StateSprite_InitCollider(struct SceneScheduledEffect *, s32, s32, s32, s32, s32);
void StateSprite_InitAnimation(struct SceneScheduledEffect *, s32, s32, s32);

struct SceneScheduledEffect *SceneScheduledEffect_Create(const struct ContactPosition *position, u32 id,
                                          s32 lifetime, u32 side, u32 type, s32 variant, s32 x,
                                          s32 y, s32 z, s32 delay, u32 flags)
{
    struct SceneScheduledEffect *effect;
    struct ScheduledEffectBody *body;
    struct ScheduledEffectSprite *sprite;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
    }
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    if (!gAnimatedBodyPool)
        return 0;
    effect = sub_0812298C(gAnimatedBodyPool);
    if (!effect)
        return 0;
    effect->position = *position;
    effect->lifetime = lifetime;
    effect->timer = 0;
    effect->delay = delay;
    effect->state = 1;
    body = &effect->body;
    SceneBody_Init(body, id, &effect->position, 0, 7, effect);
    sprite = &effect->sprite;
    SceneBody_SetSprite(body, sprite);
    sprite->flags &= ~1;
    effect->flags = flags;
    if (flags & 1) {
        if (side == 0) {
            switch ((s32)type) {
                case 0:
                    body->position.z += 0x20 + gUnk_08610710.z;
                    break;
                case 1:
                    body->position.z += 0x64 + gUnk_08610718.z;
                    break;
                case 2:
                    body->position.z += 0xc8 + gUnk_08610720.z;
                    break;
            }
        } else {
            switch ((s32)type) {
                case 0:
                    body->position.x += 0x20 + gUnk_08610710.z;
                    break;
                case 1:
                    body->position.x += 0x64 + gUnk_08610718.z;
                    break;
                case 2:
                    body->position.x += 0xc8 + gUnk_08610720.z;
                    break;
            }
        }
        TerrainOverlay_QueueAddActor(body);
    }
    StateSprite_InitCollider(effect, side, type, x, y, z);
    StateSprite_InitAnimation(effect, side, type, variant);
    return effect;
}
