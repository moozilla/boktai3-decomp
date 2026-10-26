#include "global.h"
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
    u8 pad34C[0x60];
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
static inline void effect(u8 *p, int id, int mode, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
static inline void effect2(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
void SceneActors_StopBackgroundFlash(void *);
void BackgroundFlash_BeginAppearance(void *);
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define U8(p, o)  (*((u8 *)(p) + (o)))
void SceneActorAnimation_UpdateFlashWindup(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 64, 26);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2,
                                                  1, 4);
        }
        effect((u8 *)p, 826, 1, 5);
        U32(p2, 0x3a8) = 0;
        Sound_PlaySongChecked(851);
        Sound_PlaySongChecked(887);
    }
    if (U32(p2, 0x358) != 3) {
        u32 mask = 0x100000;
        p->flags |= mask;
    }
    if (U8(p2, 0x382)) {
        {
            int age = tick >> 6;
            U8(p2, 0x399) = age;
        }
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 index = gRandomIndex * 2;
            u16 *table;
            u32 random;
            table = ((u16 *)RANDOM_TABLE_ADDR);
            index += (u32)table;
            random = *(u16 *)index;
            U8(p2, 0x398) = random;
        }
    }
    if (U16(p2, 0x254) == 5)
        U16(p2, 0x24c) = 0;
    if (p->tryUpdate(p)) {
        effect2((u8 *)p2, 2, 826, 5);
        SceneActors_StopBackgroundFlash(p2);
    } else {
        u32 mask = 64;
        p->flags |= mask;
        if (!p->reset(p)) {
            if (tick == 10)
                BackgroundFlash_BeginAppearance(*(void **)((u8 *)p2 + 0x9dc));
            if (U16(p2, 0x254) == 2 && !U16(p2, 0x24c))
                Sound_PlaySongChecked(849);
            if (tick > S16(p2, 0x374))
                p->finished = 1;
        }
    }
}
