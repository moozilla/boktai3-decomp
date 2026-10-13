#include "global.h"
typedef struct {
    s16 x, y, z, pad;
} Vec;
typedef struct {
    signed x : 16;
    signed y : 16;
    signed z : 16;
    signed pad : 16;
} PackedVec;
typedef struct {
    u8 pad[48];
    Vec position;
} Actor;
extern Actor *gUnk_02000580;
int sub_0801B54C(void *);
void sub_0801B700(void *);
void sub_0815AF14(void *, int);
void sub_082216F8(void *, void *, void *, u32, int);
void OrbitingParticles_Create(void *, int, int, int, int);
void Sound_PlaySong(int);
void sub_08017044(int, int, int, int, void *, void *, int, void *);
void sub_0815B1D0(void *, int, int);
void sub_082279A8(int, int, int, int, int, int, int);
void sub_080148CC(int, int, void *, int, int, int, int, int, int, int, int);
void sub_0801B534(void *, void (*)(void));
void Transformation_Finish(void);
void Transformation_Reveal(u8 *p)
{
    Actor *actor = gUnk_02000580;
    Vec orbit, spawn;
    PackedVec burst;
    int i, angle;
    if (sub_0801B54C(p)) {
        sub_0801B700(p);
        *(Vec *)(p + 0x28) = *(Vec *)(p + 0x18);
        *(Vec *)(p + 0x30) = actor->position;
        p[0xc1] = 4;
        sub_0815AF14(actor, 4);
    }
    if (*(u32 *)(p + 0x3c) <= 32)
        sub_082216F8(p + 0x18, p + 0x28, p + 0x30, *(u32 *)(p + 0x3c), 5);
    if (*(u32 *)(p + 0x3c) == 30) {
        orbit = actor->position;
        OrbitingParticles_Create(&orbit, 64, 60, 8, 0);
        Sound_PlaySong(0x29c);
    }
    if (*(u32 *)(p + 0x3c) == 90) {
        {
            Vec *out = &spawn;
            Vec *origin = &actor->position;
            spawn = actor->position;
            out->x -= 32;
            out->z += 32;
            out->y += 128;
            sub_08017044(1, p[0x7c], 0, 0, out, origin, 60, p);
        }
        sub_0815B1D0(actor, 0, 0);
        Sound_PlaySong(0x110);
    }
    if (*(u32 *)(p + 0x3c) == 96 && actor) {
        burst.x = actor->position.x;
        burst.y = actor->position.y + 378;
        burst.z = actor->position.z;
        sub_082279A8(0, 3, 31, 31, 31, 0xfff, 64);
        {
            PackedVec *origin = &burst;
            int spacing = 88;
            int eight = 8;
            angle = 0;
            i = 2;
            do {
                sub_080148CC(4, 0x218, origin, angle, spacing, eight, eight, eight, 1, 12, eight);
                angle += 88;
                i--;
            } while (i >= 0);
        }
    }
    if (*(u32 *)(p + 0x3c) > 163) {
        sub_0801B534(p, Transformation_Finish);
        sub_0815AF14(actor, p[0xc1]);
    } else
        (*(u32 *)(p + 0x3c))++;
}
