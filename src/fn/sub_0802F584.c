#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 99.1%
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Motion {
    u8 pad0[9];
    u8 angle;
    u8 padA[2];
    struct Vec pos;
};
struct Probe {
    u8 pad0[0xC];
    struct Vec current;
    u8 pad14[8];
    struct Vec target;
};
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define P(p, o)   ((u8 *)(p) + (o))
extern const s16 gUnk_086149C4[];
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
void Actor_SetAttachmentEffect(void *, s32, s32, s32);
void Actor_PlayFootstepSound(void *, s32);
void ProximityVolume_TranslateBounds(void *, void *);
void Terrain_ApplyBlocked(void *, void *, void *);
s32 DisplayAnimation_GetFrameBoundary(void *, u32 *);
s32 Math_Atan2(s32, s32);
void NavMesh_UpdateProbe(void *, void *);
static inline u8 *TerrainResponse(u8 *p)
{
    register s32 offset asm("r3") = 0x23C;
    asm("" ::"r"(offset));
    return p + offset;
}
static inline void ClearCounter(u8 *p, s32 value)
{
    register s32 offset asm("r4") = 0x224;
    asm("" ::"r"(offset));
    U8(p, offset) = value;
    asm("" ::"r"(offset));
}
static inline s32 Callback(u8 *p)
{
    register s32 offset asm("r1") = 0x2E4;
    asm("" ::"r"(offset));
    {
        s32 (**slot)(u8 *) = (void *)(p + offset);
        asm("" ::"r"(offset));
        return (*slot)(p);
    }
}
static inline struct Vec *ProbePosition(u8 *p)
{
    register s32 offset asm("r2") = 0x1B4;
    asm("" ::"r"(offset));
    {
        struct Vec *result = (void *)(p + offset);
        asm("" ::"r"(offset));
        return result;
    }
}
static inline s32 CurrentX(struct Probe *p)
{
    register struct Probe *raw asm("r4") = p;
    asm("" ::"r"(raw));
    return raw->current.x;
}
static inline s32 TargetX(struct Probe *p)
{
    register struct Probe *raw asm("r1") = p;
    asm("" ::"r"(raw));
    return raw->target.x;
}
static inline s32 TargetZ(struct Probe *p)
{
    register struct Probe *raw asm("r4") = p;
    asm("" ::"r"(raw));
    return raw->target.z;
}
static inline s32 PositionZ(struct Vec *p)
{
    register struct Vec *raw asm("r3") = p;
    asm("" ::"r"(raw));
    return raw->z;
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Facing(u8 *p, u16 angle)
{
    register s32 value asm("r0") = angle;
    register s32 shifted asm("r2") = value + 112;
    register s32 mask asm("r1");
    asm("" : "+r"(shifted));
    shifted >>= 5;
    mask = 7;
    mask &= shifted;
    asm("" : "+r"(mask));
    U8(p, 0x1A) = mask;
    value = (value + 96) >> 6;
    asm("" ::"r"(value));
    mask = 3;
    mask &= value;
    asm("" : "+r"(mask));
    U8(p, 0x19) = mask;
}
static inline void Polar(struct Vec *v, s32 angle, s32 speed)
{
    v->x = Scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    v->y = 0;
    v->z = Scale(speed * gUnk_086149C4[angle]);
}
// confidence=low: advances a navigation probe toward its target at animation frame boundaries.
#define SceneActorAnimation_UpdateNavigationWalk sub_0802F584
void SceneActorAnimation_UpdateNavigationWalk(void *unused, u8 *p)
{
    struct Motion *motion = (void *)P(p, 0x5C);
    register struct Probe *probe asm("r8") = (void *)P(p, 0x1A8);
    register u32 *anim asm("sb") = (void *)P(p, 0xD4);
    asm("" ::"r"(motion), "r"(probe), "r"(anim));
    if (U8(p, 9)) {
        s32 zero = 0;
        struct Vec *pos;
        U8(p, 9) = zero;
        U8(p, 7) = zero;
        Actor_PlayAnimationEntry(p, 3, 1, zero, 4);
        U8(p, 0x1F0) = 1;
        pos = (void *)P(p, 0x68);
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, TerrainResponse(p));
        ClearCounter(p, zero);
        Actor_SetAttachmentEffect(p, 0, 0, 0);
    }
    {
        u32 mask = 0x1000;
        U16(p, 0xE) |= mask;
    }
    if ((u8)Callback(p) != 1) {
        struct Vec *pos = (void *)P(p, 0x68);
        struct Vec *probePos = ProbePosition(p);
        s32 x = CurrentX(probe) - motion->pos.x;
        s32 z = probePos->z - pos->z;
        s32 distance = (s32)((u32)x * x + (u32)z * z);
        s32 limit = 0x1103;
        struct Vec *savedPos = pos;
        if (distance <= limit) {
            register struct Probe *read asm("r3") = probe;
            asm("" ::"r"(read));
            motion->pos.x = read->target.x;
            motion->pos.z = read->target.z;
            U8(p, 7) = 1;
        } else {
            u32 flags;
            if (DisplayAnimation_GetFrameBoundary(anim, &flags)) {
                register u32 *reader asm("r4") = anim;
                s32 speed;
                s32 targetX;
                s32 angle;
                struct Vec step;
                asm("" ::"r"(reader));
                if (*reader == 2) {
                    s32 flag = flags & 1;
                    speed = 64;
                    if (flag)
                        speed = 32;
                } else
                    speed = 16;
                targetX = TargetX(probe);
                targetX -= motion->pos.x;
                asm("" ::"r"(reader));
                angle = Math_Atan2(targetX, TargetZ(probe) - PositionZ(savedPos));
                U8(p, 0x65) = angle;
                Facing(p, angle);
                Polar(&step, motion->angle, speed);
                motion->pos.x += step.x;
                motion->pos.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, savedPos);
        Actor_PlayFootstepSound(p, 3);
        Actor_PlayAnimationEntry(p, 3, 1, 0, 1);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0802F584);
#endif
