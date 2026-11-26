#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/motion/scene_actor_chase.h"

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

void Centipede_ResetOscillation(void *);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Centipede_SelectWanderRoute(void *);
void Centipede_AdvanceMotionPhase(void *, s32);
void Centipede_UpdateSegmentPositions(void *, s32, s32);
void Centipede_UpdateOscillation(void *, s32, s32);

s32 Mod(s32, s32);
static inline u8 Turned(s32 angle)
{
    u8 r = 0;
    if (angle <= 31)
        r = 1;
    return r;
}
// confidence=low: spirals with decreasing speed and restores the saved facing direction.
void CentipedeBody_UpdateSpiral(struct ChaseActor *actor)
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
        s32 angle = actor->base.tracking.heading - actor->base.body.direction;
        if (angle > 128)
            angle -= 256;
        else if (angle < -128)
            angle += 256;
        if (Turned(angle)) {
            actor->base.body.direction = actor->base.tracking.heading;
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
    Centipede_UpdateSegmentPositions(actor, amplitude, rate);
    actor->base.counter2AC++;
}
