// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
extern struct Map *gTerrainMap;
extern u32 gTerrainWidth, gTerrainHeight;
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
    return gTerrainMap->cells + 12 + i * 4;
}
static inline int height(struct Vec *pos)
{
    int x = pos->x >> 8, z = pos->z >> 8, index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    return (*cell(index) & 15) << 8;
}
// confidence=low: SceneActorAnimation_UpdateWalkToPosition (sub_0802EA90) without the entry animation,
// callback test or footsteps; it moves by the new velocity itself.
void sub_081CBB1C(void *unused, struct Actor *arg)
{
    struct Actor *p = arg;
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
    }
    {
        struct Vec *pos = &p->pos;
        struct Vec *target = &p->target;
        int dx = target->x - pos->x;
        int dz = target->z - pos->z;
        u32 distance = dx * dx + dz * dz;
        u32 limit = p->speedSquared;
        struct Vec *savedPos = pos;
        struct Vec *savedTarget = target;

        if (distance <= limit) {
            p->pos = p->target;
            p->done = 1;
        } else {
            u8 *ap = &p->angle;
            struct Vec *vel = &p->velocity;
            int angle = *ap;
            int speed = p->speed;

            s32 product = speed * gUnk_086149C4[(angle + 64) & 255];
            u8 *savedAngle = ap;
            struct Vec *savedVel = vel;

            vel->x = scale(product);
            vel->y = 0;
            vel->z = scale(speed * gUnk_086149C4[angle]);
            {
                struct Vec *target = savedTarget;
                s32 x;
                struct Vec *pos;
                s32 z;

                x = target->x;
                pos = savedPos;

                x -= pos->x;

                z = target->z - pos->z;
                p->targetAngle = Math_Atan2(x, z);
            }
            {
                int value = p->targetAngle;
                u8 *out = savedAngle;

                *out = value;
                {
                    register s32 shifted asm("r2") = value + 112;
                    s32 mask;
                    asm("" : "+r"(shifted));
                    shifted >>= 5;
                    mask = 7;
                    p->facing8 = mask & shifted;
                }
                {
                    s32 mask;
                    s32 shifted = (value + 96) >> 6;
                    mask = 3;
                    mask &= shifted;
                    p->facing4 = mask;
                }
            }
            savedPos->x += savedVel->x;
            p->pos.z += p->velocity.z;
        }
    }
}
