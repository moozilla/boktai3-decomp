#include "global.h"

struct EffectPosition {
    u16 x, y, z, unk_06;
};
struct EffectDelta {
    s16 x, y, z, unk_06;
};
struct EffectTransform {
    u8 unk_00[9];
    u8 direction;
    u8 unk_0A[2];
    struct EffectPosition position;
};
struct EffectTargetDrawable {
    u8 unk_00[0x30];
    struct EffectPosition position;
};
struct EffectTarget {
    u8 unk_00[8];
    u8 kind;
    u8 unk_09[0x33];
    struct EffectTargetDrawable *drawable;
};
struct ColliderEffect {
    u32 flags;
    u8 unk_04[3];
    u8 priority;
    u8 unk_08[0x14];
    struct EffectPosition position;
    u8 unk_24[8];
    u8 animation[0x10];
    s16 velocityX, velocityY, velocityZ;
    u8 unk_42[0x44];
    u8 direction;
    u8 unk_87[0x11];
    u16 timer;
    u8 active;
    u8 index;
    struct EffectPosition *source;
};
struct ColliderEffectPool {
    u8 unk_00[0x18];
    u8 graphics[0x1C];
    void *sequence;
};
extern struct ColliderEffectPool *gUnk_0200012C;
extern const s16 gUnk_086149C4[];
struct ColliderEffectPool *SceneColliderEffect_EnsurePool(void);
struct ColliderEffect *SceneColliderEffect_AllocateSprite(struct ColliderEffectPool *);
void ActorCollider_Init(struct ColliderEffect *, u32, u32, u32);
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
u32 Direction_IsMiddleQuadrants(s32);
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);

static inline s32 TruncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

void SceneColliderEffect_SpawnAimedSprite(struct EffectTransform *from, struct EffectTarget *target, u32 a, u32 b, u32 c)
{
    struct ColliderEffectPool *loaded = gUnk_0200012C;
    struct ColliderEffectPool *pool;
    struct ColliderEffect *effect;
    struct ColliderEffect *sprite;
    struct EffectDelta delta;
    if (!loaded) {
        pool = SceneColliderEffect_EnsurePool();
        if (!pool)
            return;
    } else {
        pool = loaded;
    }
    effect = SceneColliderEffect_AllocateSprite(pool);
    if (!effect)
        return;
    effect->source = &from->position;
    effect->active = 1;
    effect->timer = 0;
    ActorCollider_Init(effect, a, b, c);
    sprite = effect;
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(sprite->animation, pool->sequence, 0, 0,
                     (u8)Direction_IsMiddleQuadrants(from->direction));
    sprite->position = from->position;
    if (target->kind > 3) {
        sprite->velocityX = 0;
        sprite->velocityY = 0;
        sprite->velocityZ = 0;
    } else {
        struct EffectTargetDrawable *drawable = target->drawable;
        struct EffectDelta *out = &delta;
        struct EffectPosition *own = &sprite->position;
        struct EffectPosition *to = &drawable->position;
        s32 timeX, timeZ;
        out->x = to->x - sprite->position.x;
        out->y = to->y - own->y;
        out->z = to->z - own->z;
        effect->direction = Math_Atan2(out->x, out->z);
        effect->velocityX = TruncateShift(gUnk_086149C4[(effect->direction + 64) & 255] * 20, 12);
        effect->velocityZ = TruncateShift(gUnk_086149C4[effect->direction] * 20, 12);
        timeX = 0;
        timeZ = 0;
        if (effect->velocityX != 0)
            timeX = Div(delta.x, effect->velocityX);
        if (effect->velocityZ != 0)
            timeZ = Div(delta.z, effect->velocityZ);
        if (timeX > timeZ)
            effect->velocityY = TruncateShift(timeX, 1);
        else
            effect->velocityY = TruncateShift(timeZ, 1);
    }
    sprite->flags |= 1;
    sprite->priority = 1;
}
