// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: selects a player-mode collision copy and completes a position request.
#define Vanargand_UpdateCaptureSetup sub_081A8E08
struct CapturePosition {
    s16 x, y, z, pad;
};
struct CaptureTarget {
    u8 pad0[0x418];
    u8 mode;
};
struct CaptureActor {
    u8 pad0[0x54];
    struct CapturePosition position;
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[0xAF - 0xA0];
    u8 active, pending, pendingAnimation, ready;
    u8 padB3[0x10C - 0xB3];
    struct CaptureTarget *target;
    u8 pad110[0x18C - 0x110];
    u8 animation[0x60];
    u8 drawable[0x7B8 - 0x1EC];
    u8 collision[3][0x20];
};
extern void *gUnk_02000580[];
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void *Player_GetCollisionData(void);
void CpuSet(const void *, void *, u32);
void Player_ApplyPositionRequest(void *, void *, s32, s32, s32, s32, s32, s32);
static inline s32 SetPositionAndGetHeight(struct CapturePosition *out, s32 x, s32 y, s32 z)
{
    // Preserve the initialized height separately from the pending-animation zero.
    register s32 height asm("r4") = y;
    out->x = x;
    (&out->y)[0] = height;
    out->z = z;
    return height;
}
static inline void AddPosition(struct CapturePosition *out, struct CapturePosition *a,
                               struct CapturePosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void Vanargand_UpdateCaptureSetup(struct CaptureActor *p)
{
    struct CaptureActor *q = p;
    struct CapturePosition position;
    u32 mask = 4;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    if (p->pendingAnimation) {
        void *animation;
        void *drawable;
        struct CapturePosition *base;
        u32 two;
        s32 animationFlags;
        u32 zero = 0;
        (&p->pendingAnimation)[0] = zero;
        animationFlags = SetPositionAndGetHeight(&position, 128, 0, 128);
        base = &position;
        AddPosition(&position, &p->position, base);
        animation = p->animation;
        drawable = p->drawable;
        two = 2;
        switch ((s32)p->target->mode) {
            case 0:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, two,
                                                     animationFlags);
                CpuSet(Player_GetCollisionData(), p->collision[0], 0x4000008);
                break;

            case 1:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, two,
                                                     animationFlags);
                CpuSet(Player_GetCollisionData(), p->collision[1], 0x4000008);
                break;

            case 2:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 14, two, zero);
                CpuSet(Player_GetCollisionData(), p->collision[2], 0x4000008);
                break;
        }
    }
    if (p->ready) {
        SceneActors_StartUnmirroredAnimation(p, q->animation, q->drawable, 3, 0, 2);
        Player_ApplyPositionRequest(gUnk_02000580[0], ((u8 *)q) + 0x1AC, 128, 32, 120, 0, 0, 0);
        p->active = 1;
    }
}
