#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "player.h"
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero = 0;
    a->field2B2 = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

struct SavedPair {
    u32 x, z;
};
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void Bee_UpdateRecoveryAnimation(void);
// confidence=low: approaches a saved player position during the recovery animation.
void Bee_ApproachSavedPlayerPosition(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *motion;
    s32 speed;
    struct SavedMotion *saved;
    s16 dx, dz;
    if (Take(&actor->field2B1)) {
        actor->direction = *((u8 *)actor + 0xE9);
        Control(actor, Bee_UpdateRecoveryAnimation, 0x26);
        *(struct SavedPair *)(body + 0x450) = *(struct SavedPair *)(((u8 **)gPlayers)[0] + 0x30);
        {
            u32 value = body[0x429];
            body[0x3DC] = value;
        }
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
        Sound_PlaySong(0x3C8);
    }
    saved = (void *)(body + 0x444);
    motion = (u8 *)actor + 0x50;
    speed = 24;
    dx = saved->position.x - *(u16 *)(motion + 12);
    dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) > 32 || (dz < 0 ? -dz : dz) > 32) {
        if (dx || dz) {
            motion[9] = Math_Atan2(dx, dz);
            *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
            *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
        }
    }
    if (actor->field2B0)
        State(actor, 2, 4);
    else
        actor->counter2AC++;
}
