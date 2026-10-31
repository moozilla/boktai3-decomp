#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
void Actor_PlayFootstepSound(void *, s32);
void ProximityVolume_TranslateBounds(void *, void *);
void Terrain_ApplyBlocked(void *, void *, void *);
s32 DisplayAnimation_GetFrameBoundary(void *, u32 *);
s32 Math_Atan2(s32, s32);
void NavMesh_UpdateProbe(void *, void *);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Polar(struct Vec *arg, s32 angle)
{
    struct Vec *v = arg;
    register s32 index asm("r4") = angle;
    register s32 speed asm("ip") = 48;
    {
        register s32 product asm("r2") = speed * gUnk_086149C4[(index + 64) & 255];
        register s32 input asm("r0");
        asm("" ::"r"(product));
        input = product;
        asm("" ::"r"(input));
        v->x = Scale(input);
    }
    v->y = 0;
    {
        register const s16 *table asm("r1") = gUnk_086149C4;
        register const s16 *entry asm("r0");
        entry = (void *)((u32)index * 2 + (u32)table);
        v->z = Scale(speed * *entry);
    }
}
static inline u8 *TerrainResponse(u8 *p)
{
    s32 offset = 0x23C;
    return p + offset;
}
static inline void SetCounter(u8 *p)
{
    s32 offset = 0x224;
    u8 *slot;
    slot = p + offset;
    *slot = 1;
}
static inline s32 Callback(u8 *p)
{
    s32 offset = 0x2E4;
    {
        s32 (**slot)(u8 *) = (void *)(p + offset);
        return (*slot)(p);
    }
}
static inline u16 ArrivalX(u8 *p)
{
    register s32 offset asm("r2") = 0x1C4;
    u16 *slot;
    slot = (void *)(p + offset);
    asm("" ::"r"(offset));
    return *slot;
}
static inline u16 ArrivalZ(u8 *p)
{
    register s32 offset asm("r3") = 0x1C8;
    u16 *slot;
    slot = (void *)(p + offset);
    asm("" ::"r"(offset));
    return *slot;
}
static inline s32 DifferenceZ(struct Vec *target, s32 current, s32 keep)
{
    s32 result = target->z - current;
    asm("" ::"r"(keep));
    return result;
}
static inline void ClearActive(u8 *p, s32 value)
{
    register s32 offset asm("r2") = 0x1F0;
    u8 *slot;
    slot = p + offset;
    asm("" ::"r"(offset));
    *slot = value;
}
static inline u32 AnimType(u32 *p)
{
    register u32 *raw asm("r4") = p;
    return *raw;
}
static inline u32 AnimFlags(u8 *p)
{
    s32 offset = 0x100;
    u8 *base;
    register u32 flags asm("r1");
    base = p + offset;
    flags = base[0x1D];
    return flags & 4;
}
static inline struct Motion *MotionOf(u8 *p)
{
    struct Motion *v = (void *)P(p, 0x5C);
    return v;
}

// confidence=low: walks toward the navigation target while the animation permits a fixed motion step.
#define SceneActorAnimation_UpdateNavigationStep sub_0802F760
void SceneActorAnimation_UpdateNavigationStep(void *unused, u8 *arg)
{
    u8 *p = arg;
    struct Motion *motion = MotionOf(p);
    struct Probe *probe = (void *)P(p, 0x1A8);
    register u32 *anim asm("r8") = (void *)P(p, 0xD4);
    if (U8(p, 9)) {
        s32 zero = 0;
        struct Vec *pos;
        U8(p, 9) = zero;
        U8(p, 7) = zero;
        Actor_PlayAnimationEntry(p, 1, 1, zero, 4);
        ClearActive(p, zero);
        pos = (void *)P(p, 0x68);
        ProximityVolume_TranslateBounds(probe, pos);
        Terrain_ApplyBlocked(probe, pos, TerrainResponse(p));
        SetCounter(p);
    }
    {
        u32 mask = 0x1000;
        U16(p, 0xE) |= mask;
    }
    if ((u8)Callback(p) != 1) {
        register struct Vec *pos asm("r2") = (void *)P(p, 0x68);
        s32 currentOffset = 0x1B4;
        register struct Vec *probePos asm("r0");
        s32 currentX;
        register s32 savedX asm("ip");
        s32 currentZ;
        s32 x;
        s32 z;
        register s32 distance asm("r1");
        s32 limit;
        struct Vec *savedPos;
        probePos = (void *)(p + currentOffset);
        x = probe->current.x;
        currentX = motion->pos.x;
        savedX = currentX;
        asm("" ::"r"(savedX));
        x -= currentX;
        {
            register s32 probeZ asm("sb") = probePos->z;
            currentZ = pos->z;
            z = probeZ - currentZ;
        }
        {
            register u32 xx asm("r4") = (u32)x * x;
            {
                register u32 squareZ asm("r1") = (u32)z * z;
                register u32 addend asm("r0");
                asm("" ::"r"(squareZ));
                addend = squareZ;
                asm("" ::"r"(addend));
                distance = (s32)(xx + addend);
            }
        }
        limit = 0x9C3;
        savedPos = pos;
        if (distance <= limit) {
            motion->pos.x = ArrivalX(p);
            motion->pos.z = ArrivalZ(p);
            U8(p, 7) = 1;
        } else {
            if (AnimType(anim) == 2 && AnimFlags(p)) {
                register s32 targetOffset asm("r2") = 0x1C4;
                struct Vec *target;
                s32 x;
                s32 fromX;
                s32 angle;
                struct Vec step;
                target = (void *)(p + targetOffset);
                x = probe->target.x;
                asm("" ::"r"(targetOffset));
                fromX = savedX;
                angle = Math_Atan2(x - fromX, DifferenceZ(target, currentZ, fromX));
                U8(p, 0x65) = angle;
                {
                    u16 narrowed = angle;
                    s32 value = narrowed;
                    s32 shifted = value + 112;
                    register s32 mask asm("r1");
                    shifted >>= 5;
                    mask = 7;
                    mask &= shifted;
                    U8(p, 0x1A) = mask;
                    value = (value + 96) >> 6;
                    mask = 3;
                    mask &= value;
                    U8(p, 0x19) = mask;
                }
                Polar(&step, motion->angle);
                {
                    register u32 delta asm("r0") = (u16)step.x;
                    register u32 old asm("r2");
                    old = (u16)motion->pos.x;
                    motion->pos.x = delta + old;
                }
                {
                    register u32 delta asm("r0") = (u16)step.z;
                    register u32 old asm("r3");
                    old = (u16)motion->pos.z;
                    motion->pos.z = delta + old;
                }
            }
        }
        NavMesh_UpdateProbe(probe, savedPos);
        Actor_PlayFootstepSound(p, 1);
        Actor_PlayAnimationEntry(p, 1, 1, 0, 1);
    }
}
