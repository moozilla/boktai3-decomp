#include "global.h"
struct PhaseSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6, pad7;
    s8 scaleX, scaleY;
};
struct PhaseCollision {
    u8 pad0[6];
    u16 flags;
};
struct PhaseActor {
    u8 pad0[4];
    struct PhaseSprite *sprite;
    u8 pad8[0xfc];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x1c];
    struct PhaseCollision collision134;
    u8 pad13c[0x4c];
    struct PhaseCollision collision188;
    u8 pad190[0x8c];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0xa];
    u16 flags2c0;
    u8 pad2c2[6];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};
static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}
static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

struct PhaseVector {
    s32 x, y;
};
void ActorShadow_Disable(void *);
void Sound_PlaySong(u32);
void ActorPhase_EmitTrailSprite(struct PhaseActor *, void *, u32);
void Axe_UpdateTrailColliders(struct PhaseActor *);
void Axe_UpdateAttackColliderPositions(struct PhaseActor *);
void Axe_ReleaseAuxiliarySprite(struct PhaseActor *);
void SceneActorControl_ResetRotation(void);
void ActorPhase_UpdateAcceleratingTrail(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    if (consumeEntry(s)) {
        *(u32 *)(data + 0xa38) |= 2;
        data[0xcab] = 0;
        data[0xa35] = 0;
        *(struct PhaseVector *)(data + 0xa24) = *(struct PhaseVector *)(data + 0xa54);
        {
            u32 angle = data[0xca8];
            data[0xa34] = angle;
        }
        data[0xa36] = 4;
        ActorShadow_Disable(data + 0xcac);
        *(u32 *)(data + 0xa38) = 1;
    }
    {
        s32 angle = data[0xa34];
        s32 next = angle - data[0xa36];
        data[0xa34] = next;
        if (angle < next)
            Sound_PlaySong(0x169);
    }
    if ((1 & frame) && data[0xa36] <= 19)
        data[0xa36]++;
    ActorPhase_EmitTrailSprite(s, data + 0xa24, data[0xa34]);
    Axe_UpdateTrailColliders(s);
    Axe_UpdateAttackColliderPositions(s);
    if ((*(u32 *)(data + 0xa38) & 1) && frame > 30) {
        Axe_ReleaseAuxiliarySprite(s);
        {
            void (*next)(void) = SceneActorControl_ResetRotation;
            u32 id = 0x34;
            s->entered = 1;
            s->phaseState = 0;
            s->phaseId = id;
            s->phase = next;
            s->timer = 0;
            clearFlags(&s->flags114, 1);
            s->f104 = 0;
        }
    }
}
