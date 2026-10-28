#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
u32 Slime_GetRandomVariantFlags(void *);
s32 Math_Atan2(s32, s32);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
static inline u16 Random(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Delta(struct ContactPosition *v, struct ContactPosition *from,
                         struct ContactPosition *to)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
}
static inline u16 *Palette(struct SceneActorRegistryResetView *actor)
{
    return &actor->basePaletteBank;
}
static inline u16 *Offset(struct SceneActorRegistryResetView *actor)
{
    return &actor->paletteOffset;
}
static inline u16 *Power(struct SceneActorRegistryResetView *actor)
{
    return &actor->fieldB6;
}
static inline u16 *Parameter(struct SceneActorRegistryResetView *actor)
{
    return &actor->fieldBE;
}
static inline u32 *Flags(struct SceneActorRegistryResetView *actor)
{
    return &actor->wordD0;
}
// confidence=low: launches three randomized reaction projectiles toward the current target.
void Slime_EmitTargetedReactionBurst(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s16 jitter = Random() & 15;
    Delta(&velocity, &actor->position, (void *)(actor->fieldDC + 12));
    angle = Math_Atan2(velocity.x, velocity.z);
    Polar(&velocity, angle + jitter, (Random() & 7) + 6);
    velocity.y = 48;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Palette(actor);
        offset = Offset(actor);
        sum = *palette + *offset;
        power = Power(actor);
        attack = *power;
        parameter = Parameter(actor);
        response = *parameter;
        flags = Flags(actor);
        bits = *flags | variant;
        value = &actor->fieldC6;
        v = *value;
        position = &actor->position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Random() & 15;
    Polar(&velocity, angle + jitter, (Random() & 7) + 6);
    velocity.y = 37;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Palette(actor);
        offset = Offset(actor);
        sum = *palette + *offset;
        power = Power(actor);
        attack = *power;
        parameter = Parameter(actor);
        response = *parameter;
        flags = Flags(actor);
        bits = *flags | variant;
        value = &actor->fieldC6;
        v = *value;
        position = &actor->position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 7, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Random() & 15;
    Polar(&velocity, angle + jitter, (Random() & 7) + 6);
    velocity.y = 32;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Palette(actor);
        offset = Offset(actor);
        sum = *palette + *offset;
        power = Power(actor);
        attack = *power;
        parameter = Parameter(actor);
        response = *parameter;
        flags = Flags(actor);
        bits = *flags | variant;
        value = &actor->fieldC6;
        v = *value;
        position = &actor->position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 1, 0, attack, response,
                     bits, v);
    }
}
