// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.2%

// 32 differing instruction runs; size +4 bytes; first diff at +0xA (adds r5, r1, #0 vs adds r4, r1, #0)
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[7];
    u8 done;
    u8 pad8[1];
    u8 entered;
    u8 pada[4];
    u16 flags;
    u8 pad10[8];
    u8 phase;
    u8 facing4;
    u8 facing8;
    u8 targetAngle;
    u8 pad1c[56];
    struct Vec target;
    u8 body[9];
    u8 angle;
    u8 pad66[2];
    struct Vec pos;
    struct Vec velocity;
    u8 pad78[92];
    u32 drawable;
    u8 padd8[80];
    struct Vec *sourcePos;
    u8 pad12c[52];
    struct Vec *drawPos;
    u8 pad164[164];
    int speed;
    u32 speedSquared;
    u8 pad210[212];
    int (*callback)(struct Actor *);
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *TerrainModifiers_FindCellOverride(int, int);
extern const s16 gUnk_086149C4[];
int Actor_PlayAnimationEntry(struct Actor *, int, int, int, int);
void Actor_PlayFootstepSound(struct Actor *, int);
void DisplayAnimation_SetFlags(void *, int);
void DisplayAnimation_ClearFlags(void *, int);
void DisplayAnimation_SetPalette(void *, int, int, int);
void Sound_PlaySong(int);
int Math_Atan2(int, int);
static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
static inline int height(struct Vec *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    return (*cell(index) & 15) << 8;
}
// confidence=low: advances toward the scripted position using the current movement angle.
#define SceneActorAnimation_UpdateWalkToPosition sub_0802EA90
void SceneActorAnimation_UpdateWalkToPosition(void *unused, struct Actor *arg)
{
    register struct Actor *p asm("r5") = arg;
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
        Actor_PlayAnimationEntry(p, 1, 1, zero, 4);
    }
    if ((u8)p->callback(p) != 1) {
        register struct Vec *pos asm("r3") = &p->pos;
        struct Vec *target = &p->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = p->speedSquared;
        register struct Vec *savedPos asm("r8") = pos;
        register struct Vec *savedTarget asm("ip") = target;

        if (distance <= limit) {
            p->pos = p->target;
            p->done = 1;
        } else {
            register u8 *ap asm("r3") = &p->angle;
            register struct Vec *vel asm("r4") = &p->velocity;
            register int angle asm("r6") = *ap;
            int speed = p->speed;

            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            register u8 *savedAngle asm("sb") = ap;

            vel->x = scale(product);
            vel->y = 0;
            vel->z = scale(speed * gUnk_086149C4[angle]);
            asm("" ::"r"(angle), "r"(speed));
            {
                register struct Vec *target asm("r3") = savedTarget;
                s32 x;
                register struct Vec *pos asm("r6");
                s32 z;

                x = target->x;
                pos = savedPos;

                x -= pos->x;

                z = target->z - pos->z;
                p->targetAngle = Math_Atan2(x, z);
            }
            {
                int value = p->targetAngle;
                register u8 *out asm("r4") = savedAngle;

                *out = value;
                {
                    register s32 shifted asm("r2") = value + 112;
                    register s32 mask asm("r0");
                    asm("" : "+r"(shifted));
                    shifted >>= 5;
                    mask = 7;
                    asm("" ::"r"(shifted), "r"(mask));
                    p->facing8 = mask & shifted;
                }
                {
                    register s32 mask asm("r0");
                    s32 shifted = (value + 96) >> 6;
                    asm("" ::"r"(shifted));
                    mask = 3;
                    asm("" ::"r"(mask));
                    mask &= shifted;
                    asm("" : "+r"(mask));
                    p->facing4 = mask;
                }
            }
            Actor_PlayAnimationEntry(p, 1, 1, 0, 1);
        }
        Actor_PlayFootstepSound(p, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0802EA90);
#endif
