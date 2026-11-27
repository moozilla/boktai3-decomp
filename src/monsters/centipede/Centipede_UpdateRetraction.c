#include "global.h"
#include "monsters/centipede/centipede_body.h"

// confidence=low: state advances segment interpolation and oscillation before restoring control.

// confidence=low: inferred from segment state and collider updates.

void Centipede_SelectWanderRoute(void *);
void Centipede_UpdateSegmentPositions(void *, s32, s32);
void Centipede_UpdateOscillation(void *, s32, s32);
static inline void Advance(void *p, s32 x, s32 z)
{

    u16 k[] = {0x6400};
    Centipede_UpdateSegmentPositions(p, 4, k[0]);
}
union RetractionActorHandle {
    struct ChaseActor *ptr;
    u8 parts[4];
};
void Centipede_UpdateRetraction(struct ChaseActor *input)
{
    union RetractionActorHandle actor;
    u32 raw;
    actor.ptr = input;
    if (SceneActorControl_TakeStateEntry(actor.ptr)) {
        SceneActorChase_SetControl(actor.ptr, CentipedeBody_UpdateChargeSpread, 0x23);
        actor.ptr->base.field2E8 = 8;
    }
    {
        s32 x = actor.ptr->base.secondaryMotion.x;
        s32 ax = x;
        s32 z;
        s32 az;
        if (x < 0)
            ax = -x;
        z = actor.ptr->base.secondaryMotion.z;
        az = z;
        if (z < 0)
            az = -z;
        if (ax > az) {
            if (x < 0) {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            } else {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            }
        } else {
            if (z < 0) {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            } else {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            }
        }
    }
    Centipede_UpdateOscillation(actor.ptr, 0x1ae, 0x28);
    if (actor.ptr->base.stepDone) {
        Centipede_SelectWanderRoute(actor.ptr);
        SceneActorChase_SetControl(actor.ptr, actor.ptr->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor.ptr, 2);
    } else {
        actor.ptr->base.counter2AC++;
    }
}
