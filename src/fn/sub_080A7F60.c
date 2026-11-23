#include "scene_actor_chase_reaction.h"
#include "scene_actor_init_actor.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_chase.h"

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);

static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline s32 Shift(s32 n, u32 bits)
{
    s32 r;
    if (n >= 0)
        r = n >> bits;
    else
        r = -((-n) >> bits);
    return r;
}

struct SavedPair {
    u32 x, z;
};
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void TentacleActor_ResetOscillation(void *);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void TentacleActor_SelectWanderRoute(void *);
void TentacleActor_AdvanceMotionPhase(void *, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
static inline u8 Approach(struct ChaseActor *actor, struct SavedMotion *saved, s32 speed)
{
    u8 *motion = (u8 *)actor + 0x50;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}

s32 Mod(s32, s32);
static inline u8 Turned(s32 angle)
{
    u8 r = 0;
    if (angle <= 31)
        r = 1;
    return r;
}
// confidence=low: spirals with decreasing speed and restores the saved facing direction.
void TentacleBody_UpdateSpiral(struct ChaseActor *actor)
{
    s32 speed;
    s32 amplitude, turn, rate;
    s32 dx, dz;
    if (Take(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        MonsterInit_SetStatusFlags(actor, 0x100000);
    }
    if (Mod(actor->base.counter2AC, 48) == 0)
        Sound_PlaySong(0x3D7);
    ZeroFill8(&actor->base.secondaryMotion);
    if ((s32)actor->base.counter2AC > 115) {
        s32 angle = *((u8 *)actor + 0xE9) - actor->base.body.direction;
        if (angle > 128)
            angle -= 256;
        else if (angle < -128)
            angle += 256;
        if (Turned(angle)) {
            actor->base.body.direction = *((u8 *)actor + 0xE9);
            SceneActorChase_SetState(actor, 0, 0, 6);
            return;
        }
    }
    if ((s32)actor->base.counter2AC <= 63) {
        speed = 20;
        amplitude = 8;
        turn = 4;
        rate = 0x1860;
    } else if ((s32)actor->base.counter2AC <= 115) {
        speed = 14;
        amplitude = 8;
        turn = 8;
        rate = 0x4D8;
    } else {
        speed = 3;
        amplitude = 8;
        turn = 6;
        rate = 0x4D8;
    }
    dx = Shift(gUnk_086149C4[(actor->base.body.direction + 64) & 255] * speed, 12);
    dz = Shift(gUnk_086149C4[actor->base.body.direction] * speed, 12);
    actor->base.body.direction += turn;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    sub_080A9DB0(actor, amplitude, rate);
    actor->base.counter2AC++;
}
