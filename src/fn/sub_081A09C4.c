#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define Hresvelgr_PlayDescent sub_081A09C4
typedef struct OrbitPosition {
    s16 x, y, z, pad;
} OrbitPosition;
typedef struct OrbitActor {
    u8 pad0[0x54];
    OrbitPosition position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[15];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 pulse;
    u8 padB3[0xc1];
    u8 animationData[0x20];
    u8 animation[0x20];
    u8 pad1B4[0x1e8];
    OrbitPosition savedPosition;
} OrbitActor;
void ActionState_Reset(u8 *);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_RefreshTarget(OrbitActor *);
void Hresvelgr_SetAnimation(OrbitActor *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void OffsetActor_StepTowardTarget(OrbitActor *);
void Sound_PlaySong(u32);
void Hresvelgr_PlayDescent(void *p, int tick)
{
    OrbitActor *actor = p;
    asm("" : "+r"(actor));
    {
        u32 mask = 0x200000;
        actor->flags |= mask;
        actor->flags |= 12;
    }
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_RefreshTarget(actor);
        actor->position = actor->savedPosition;
        actor->position.y = 0x2000;
        Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 0, 1, 0);
        Anim_SetFrameDurationScale(actor->animation, 128);
    }
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    OffsetActor_StepTowardTarget(actor);
    if (actor->position.y <= 0x340) {
        actor->position.y = 0x340;
        actor->finished = 1;
    } else if (!(tick & 31))
        Hresvelgr_RefreshTarget(p);
}
