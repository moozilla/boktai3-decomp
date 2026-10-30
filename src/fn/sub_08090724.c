#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
u32 Slime_GetRandomVariantFlags(void *);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
static inline u32 Slime_RandomSample(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline s32 Slime_TruncateFixed12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void Slime_SetPolarMotion(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Slime_TruncateFixed12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Slime_TruncateFixed12(gUnk_086149C4[angle & 255] * radius);
}
static inline void Slime_SubtractPositions(struct ContactPosition *v, struct ContactPosition *from,
                                           struct ContactPosition *to)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
}
static inline u16 *Slime_PaletteBase(struct SceneActorRegistryResetView *actor)
{
    return &actor->basePaletteBank;
}
static inline u16 *Slime_PaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return &actor->paletteOffset;
}
static inline u16 *Slime_AttackPower(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.first[1];
}
static inline u16 *Slime_AttackResponse(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.second[1];
}
static inline u32 *Slime_AttackFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.flags[1];
}
static inline u16 *Slime_AttackParameter(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.parameter[1];
}
static inline struct ContactPosition *Slime_Position(struct SceneActorRegistryResetView *actor)
{
    return &actor->body.position;
}
/* Emit three reaction projectiles with independent random headings. */
void Slime_EmitScatteredReactionBurst(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s32 jitter = Slime_RandomSample() & 15;
    Slime_SubtractPositions(&velocity, &actor->body.position, &actor->tracking.target->position);
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 12);
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
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_RandomSample() & 15;
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 12);
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
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 7, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_RandomSample() & 15;
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 6);
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
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 1, 0, attack, response,
                     bits, v);
    }
}
