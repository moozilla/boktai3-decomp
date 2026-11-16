#include "rotation_phase.h"
#include "player.h"

// confidence=low: launches toward the player and brakes sprite rotation on arrival.
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Axe_EmitMotionTrail(struct ChaseActor *actor);
void sub_080FC094(struct ChaseActor *actor, s32 count);
void Axe_SetAuxiliarySpriteRenderState(struct S080FB958 *s, u32 x, u32 y);
void SceneActorControl_FinishRotation(struct ChaseActor *actor, s32 elapsed);
static inline s32 Axe_ShiftSigned(s32 v)
{
    return v >= 0 ? v >> 6 : -(-v >> 6);
}
static inline u32 Axe_TestFlags(u32 k_, u32 *flags)
{
    return (*flags) & k_;
}

inline static void Axe_BeginRotationControl(struct ChaseActor *a, void *callback, u8 phase)
{
    u32 zero = 0;
    {
        typeof(a->base.field2B2) *q1 = &a->base.field2B2;
        *q1 = 1;
    }
    a->base.stepDone = zero;
    a->base.controlKind = phase;
    a->base.target = callback;
    a->base.field2C8 = zero;
    {
        u8 k1 = 1;
        a->base.pendingControlFlags &= ~k1;
    }
    a->base.pendingControl = zero;
}

struct AxeLungePlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
};

static inline void Axe_DeltaFromPlayer(u8 **player, struct ContactPosition *v,
                                       struct ContactPosition *b)
{
    struct ContactPosition *a = &((struct AxeLungePlayer *)*player)->position;
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}

struct AxeLungeState {
    u8 unknown0[0x8A8];
    u8 auxiliary[0xE48 - 0x8A8];
    s32 healthThreshold;
    u32 flags;
    u8 unknownE50[0xEAE - 0xE50];
    u8 angularSpeed;
    u8 unknownEAF[3];
    u8 trailDuration;
    u8 delay;
    u8 unknownEB4[0xED1 - 0xEB4];
    u8 queued;
    u8 unknownED2;
    u8 angle;
};

void Axe_UpdateTargetLunge(struct ChaseActor *a, s32 frame)
{
    struct AxeLungeState *state = a->base.context;
    struct RotatingDrawable *sprite = a->base.drawable;
    u8 entered = SceneActorControl_TakeFlag(&a->base.field2B2);
    s32 angle;
    u32 *flags = &a->base.statusFlags;
    if (entered) {
        struct ContactPosition delta;
        sprite->flags |= 2;
        state->angularSpeed = 2;
        Axe_DeltaFromPlayer(&gPlayers, &delta, &a->base.body.position);
        if (a->base.health <= (state->healthThreshold)) {
            a->base.secondaryMotion.x = Div(delta.x, 44);
            a->base.secondaryMotion.y = 44;
            a->base.secondaryMotion.z = Div(delta.z, 44);
        } else {
            a->base.secondaryMotion.x = Axe_ShiftSigned(delta.x);
            a->base.secondaryMotion.y = 64;
            a->base.secondaryMotion.z = Axe_ShiftSigned(delta.z);
        }
        state->delay = 0;
        {
            u32 *entryFlags = &a->base.statusFlags;
            u32 mask = ~0x100000;
            *entryFlags &= mask;
            Axe_SetAuxiliarySpriteRenderState(a, state->auxiliary, 0x13D);
            Sound_PlaySong(0x1E5);
            flags = entryFlags;
        }
    }
    a->base.body.position.y += a->base.heightOffset;
    if (NormalizeAngle(32 - state->angle) < 0)
        sprite->angle += state->angularSpeed;
    else
        sprite->angle -= state->angularSpeed;
    angle = NormalizeAngle(sprite->angle);
    Axe_EmitMotionTrail(a);
    if (frame <= 27) {
        if (!Mod(frame, 10))
            state->angularSpeed += 4;
    } else {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= 95)
            state->angularSpeed = 8;
        else
            state->angularSpeed = 0;
    }
    if (Axe_TestFlags(0x10, flags)) {
        state->queued = 1;
        Axe_BeginRotationControl(a, SceneActorControl_FinishRotation, 0x33);
        {
            u16 k1 = 0x800;
            state->flags &= ~k1;
        }
        state->trailDuration = 64;
        sub_080FC094(a, 14);
    }
}
