// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x48];
    u8 type;
    u8 pad395[0x17];
    u32 mode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    int shifted;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(int);
int Math_Atan2(int, int);
extern s16 gUnk_086149C4[];
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);
typedef struct {
    u32 a, b;
} Point;
void SceneActorAnimation_UpdateHomingLunge(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u32 *savedFlags;
    u8 *position = (u8 *)p + 0x48;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 52, 14);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x240, (u8 *)p + 0x220, sequence, 2, 3,
                                              4);
        Sound_PlaySongChecked(842);
    }
    {
        register u32 mask asm("r2");
        if (p2->type == 1)
            mask = 8;
        else
            mask = 0x40000;
        {
            register u32 *flags asm("r1") = &p->flags;
            *flags |= mask;
            savedFlags = flags;
        }
    }
    if (p->tryUpdate(p))
        return;
    {
        u32 mask = 64;
        *savedFlags |= mask;
    }
    if (p->reset(p))
        return;
    if (!(tick & 3)) {
        if (!p2->type) {
            int id = SceneActorGraphics_GetResourceId(p2);
            u8 *data = gPaletteBankData + id * 32;
            {
                register int offset asm("r4") = 0x240;
                asm("" ::"r"(offset));
                TemporaryAnimationPool_CreateEntry((u8 *)p2 + offset, 8, 0x200, 0x9000, data);
            }
        } else {
            int offset;
            u8 *data;
            data = gPaletteBankData;
            offset = 0x6740;
            asm("" ::"r"(offset));
            data += offset;
            {
                register int drawOffset asm("r2") = 0x240;
                asm("" ::"r"(drawOffset));
                TemporaryAnimationPool_CreateEntry((u8 *)p2 + drawOffset, 8, 0x200, 0x9000, data);
            }
        }
    }
    if (p->locked) {
        s16 *dest = (s16 *)((u8 *)p2 + 0x9c4);
        int dx = dest[0] - *(s16 *)(position + 12), dz = dest[2] - *(s16 *)(position + 16);
        if (dx * dx + dz * dz < 1600) {
            *(Point *)(position + 12) = *(Point *)dest;
            goto finished;
        }
    }
    {
        s16 *velocity = (s16 *)(position + 20);
        u32 angle = position[9];
        int speed;
        speed = 40;
        {
            int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
            int zero = 0;
            velocity[0] = x;
            velocity[1] = zero;
        }
        velocity[2] = trunc12(gUnk_086149C4[angle] * speed);
    }
    if (tick > 159) {
    finished:
        p->finished = 1;
    }
}
