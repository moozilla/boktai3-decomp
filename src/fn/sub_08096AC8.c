#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#define FIELD(p, o, t) (*(t *)((u8 *)(p) + (o)))
extern const s16 gUnk_086149C4[];
void SceneActorControl_OnIdle(void *);
void CpuSet(const void *, void *, u32);
s32 Math_Atan2(s32, s32);
void Ghost_UpdateRecoilAnimation(void *);
u8 Ghost_SelectRandomRouteEntry(void *);
u8 sub_08097798(void *);
void Bee_ConsumeFlag80(void *);
void Bee_ClearActionFlags(void *);
void Ghost_StartMovementToPosition(void *, void *);
void Ghost_SetMovementEndpoints(void *, void *);
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
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
static inline void State(struct SceneActorRegistryResetView *a, u32 state, u32 phase, u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline s32 Scale(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle] * speed);
}
static inline void Endpoint(u8 *path, struct ContactPosition *pos)
{
    u16 *point = FIELD(path, 8, u16 *);
    pos->x = point[0];
    pos->z = FIELD(path, 8, u16 *)[1];
    pos->y = SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
}
static inline void Clear(u8 *base, u32 off, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, base + off, 0x05000002);
}
static inline void Clear2(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
// confidence=low: starts a random recoil then returns to the stored terrain endpoint.
void Ghost_RecoilTowardEndpoint(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    struct ContactPosition pos;
    u32 zero;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 angle;
        u32 n;
        Control(actor, Ghost_UpdateRecoilAnimation, 0x24);
        Bee_ConsumeFlag80(actor);
        gRandomIndex = n = (gRandomIndex + 1) & 1023;
        angle = *(u8 *)&((const u16 *)RANDOM_TABLE_ADDR)[n];
        {
            u32 off = 0x5FC;
            Clear2(context + off, &zero);
        }
        FIELD(actor, 0x2EC, s16) = Scale(gUnk_086149C4[angle] * 64);
        FIELD(actor, 0x2F0, s16) = Scale(gUnk_086149C4[(angle + 64) & 255] * 64);
        actor->direction = Math_Atan2(FIELD(actor, 0x2EC, s16), FIELD(actor, 0x2F0, s16)) - 128;
    }
    if (actor->field2B0) {
        Bee_ClearActionFlags(actor);
        Clear2((u8 *)actor + 0x2EC, &zero);
        State(actor, 0, 0, 2);
        Control(actor, SceneActorControl_OnIdle, 16);
        {
            u8 *path = context + 0x60C;
            Ghost_SelectRandomRouteEntry(path);
            Endpoint(path, &pos);
        }
        Ghost_StartMovementToPosition(actor, &pos);
    } else
        ++actor->counter2AC;
}
