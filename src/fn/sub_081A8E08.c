// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: stack-vector aliases, zero lifetimes and constant scheduling.
// confidence=low: selects a player-mode collision copy and completes a position request.
#define Vanargand_UpdateCaptureSetup sub_081A8E08
struct CaptureTarget {
    u8 pad0[0x418];
    u8 mode;
};
struct CaptureActor {
    u8 pad0[0x54];
    s16 position[4];
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
static inline void SetPosition(s16 *out, s32 x, s32 y, s32 z)
{
    out[0] = x;
    out[1] = y;
    out[2] = z;
}
static inline void AddPosition(s16 *out, s16 *a, s16 *b)
{
    s8 av = a[0];
    u16 bv = b[0];
    typeof(bv) c1;
    out[0] = av + bv;
    av = a[1];
    bv = b[1];
    c1 = bv;
    out[1] = a[1] + c1;
    av = a[2];
    bv = b[2];
    out[2] = a[2] + c1;
}

inline static void SetPosition_081A8E08(s16 *out, s32 x, s32 y, s32 z)
{
    out[1] = y;
    out[0] = x;
    out[2] = z;
}

inline static void AddPosition_081A8E08(s16 *out, s16 *a, s16 *b)
{
    s8 av = a[0];
    u16 bv = b[0];
    typeof(bv) c1;
    out[0] = av + bv;
    av = a[1];
    do {
        bv = b[1];
    } while (0);
    c1 = bv;
    out[1] = a[1] + c1;
    av = a[2];
    bv = b[2];
    out[2] = a[2] + c1;
}

void Vanargand_UpdateCaptureSetup(struct CaptureActor *p)
{
    register struct CaptureActor *q asm("r6") = p;
    s16 position[4];
    u32 mask = 4;
    asm("" : "+r"(q));
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    if (p->pendingAnimation) {
        void *animation;
        void *drawable;
        s16 *base;
        u32 two;
        register u32 zero asm("r12") = 0;
        asm("" : "+r"(zero));
        do {
            p->pendingAnimation = zero;
        } while (0);
        SetPosition_081A8E08(position, 128, 0, 128);
        base = position;
        asm("" : "+r"(base));
        AddPosition_081A8E08(position, p->position, base);
        two = 2;
        animation = p->animation;
        drawable = p->drawable;
        switch ((s32)p->target->mode) {
            case 0:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, two, 0);
                CpuSet(Player_GetCollisionData(), p->collision[0], 0x4000008);
                break;

            case 1:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, two, 0);
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

#else
INCLUDE_ASM("asm/nonmatching", sub_081A8E08);
#endif
