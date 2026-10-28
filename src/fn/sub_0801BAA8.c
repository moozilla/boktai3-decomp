#include "global.h"
#include "player.h"
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
int ResultMenu_IsPhaseStart(void *);
void sub_0801B700(void *);
void PlayerCue_Stand(void *, int);
void Vec16_Lerp(void *, void *, void *, u32, int);
void OrbitingParticles_Create(void *, int, int, int, int);
void Sound_PlaySong(int);
void FloatingItemEffect_Spawn(int, int, int, int, void *, void *, int, void *);
void PlayerCue_Motion7Front(void *, int, int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void sub_080148CC(int, int, void *, int, int, int, int, int, int, int, int);
void ResultMenu_SetPhase(void *, void (*)(void));
void Transformation_Finish(void);
void Transformation_Reveal(u8 *p)
{
    Actor *actor = (*(Actor **)gPlayers);
    Vec orbit, spawn;
    PackedVec burst;
    int i, angle;
    if (ResultMenu_IsPhaseStart(p)) {
        sub_0801B700(p);
        *(Vec *)(p + 0x28) = *(Vec *)(p + 0x18);
        *(Vec *)(p + 0x30) = actor->position;
        p[0xc1] = 4;
        PlayerCue_Stand(actor, 4);
    }
    if (*(u32 *)(p + 0x3c) <= 32)
        Vec16_Lerp(p + 0x18, p + 0x28, p + 0x30, *(u32 *)(p + 0x3c), 5);
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
            FloatingItemEffect_Spawn(1, p[0x7c], 0, 0, out, origin, 60, p);
        }
        PlayerCue_Motion7Front(actor, 0, 0);
        Sound_PlaySong(0x110);
    }
    if (*(u32 *)(p + 0x3c) == 96 && actor) {
        burst.x = actor->position.x;
        burst.y = actor->position.y + 378;
        burst.z = actor->position.z;
        ScreenTransition_SendCommand(0, 3, 31, 31, 31, 0xfff, 64);
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
        ResultMenu_SetPhase(p, Transformation_Finish);
        PlayerCue_Stand(actor, p[0xc1]);
    } else
        (*(u32 *)(p + 0x3c))++;
}
