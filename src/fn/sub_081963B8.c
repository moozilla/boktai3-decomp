#include "global.h"
#include "game_state.h"

struct P081963B8 {
    u8 f0[0x71be];
    u8 b71be;
    u8 f1[0x7244 - 0x71bf];
    s16 timer;
    u8 f2[0x724a - 0x7246];
    u8 state;
    u8 f3[2];
    u8 b724d;
    u8 b724e;
    u8 f4[0x7276 - 0x724f];
    s16 h7276;
    u8 f5[0x728a - 0x7278];
    s16 h728a;
    s16 h728c;
};
extern u32 gUnk_030053F4, gUnk_030053F0;
extern u32 gUnk_02000560;
void Nidhoggr_StartSuspendedObjectDescent(void *);
s32 Nidhoggr_IsSuspendedObjectReady(void *);
void Camera_RequestShake(u32);
void Nidhoggr_RegisterTerrainCells(void *);
void Nidhoggr_SetConfiguredTargetValues(void *);
void Nidhoggr_ClearTargetValues(void *);
void Nidhoggr_StartSuspendedObjectShake(void *);
void Nidhoggr_UnregisterTerrainCells(void *);
void Nidhoggr_SetTerrainCellFlag40(void *);
void Nidhoggr_StartSuspendedObjectRetraction(void *);
s32 Nidhoggr_IsSuspendedObjectHidden(void *);
void Nidhoggr_SetBehavior(void *, u32);
void SceneAnimation_InitTimedCollider(void *);
void Sound_PlaySongChecked(u32);
void Sound_StopSong(u32);

static inline u32 Tst2(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}

static inline u32 Tst32(u32 mask, u32 *p)
{
    return *p & mask;
}

static inline void Track(struct P081963B8 *t)
{
    if (gUnk_02000710->positionX > 0xeff)
        Nidhoggr_SetConfiguredTargetValues(t);
    else
        Nidhoggr_ClearTargetValues(t);
}

void Nidhoggr_UpdateTimedAttackSequence(struct P081963B8 *t)
{
    switch (t->state) {
        case 0:
            Nidhoggr_StartSuspendedObjectDescent(t);
            t->state++;
            break;
        case 1:
            if (Nidhoggr_IsSuspendedObjectReady(t)) {
                t->state++;
                t->timer = 0;
            }
            break;
        case 2:
            if (t->timer <= 9) {
                Camera_RequestShake(0x14);
                break;
            }
            Nidhoggr_RegisterTerrainCells(t);
            t->timer = 0;
            t->state++;
            t->b71be = 1;
            break;
        case 3:
            Track(t);
            if (t->timer > 0x1a3) {
                Nidhoggr_StartSuspendedObjectShake(t);
                Sound_PlaySongChecked(0x1c1);
                t->state++;
            }
            break;
        case 4:
            Track(t);
            if (!(t->timer & 7))
                t->b724e |= 3;
            if (t->timer > 0x257) {
                t->b71be = 0;
                if (gUnk_02000710->positionX > 0xeff)
                    Nidhoggr_UnregisterTerrainCells(t);
                else
                    Nidhoggr_SetTerrainCellFlag40(t);
                Nidhoggr_StartSuspendedObjectRetraction(t);
                Sound_StopSong(0x1c1);
                t->state++;
            }
            break;
        case 5:
            Track(t);
            t->b724e |= 3;
            if (Nidhoggr_IsSuspendedObjectHidden(t))
                t->state++;
            break;
        case 6:
            if (gUnk_02000710->positionX > 0xeff) {
                if ((t->h728a <= 0 && t->h728c <= 0) || t->h7276 <= 0)
                    Nidhoggr_SetBehavior(t, 12);
                else
                    Nidhoggr_SetBehavior(t, 10);
            } else {
                SceneAnimation_InitTimedCollider(t);
                t->state++;
            }
            break;
        case 7:
            if (!t->b724d) {
                if (!Tst2(0x400, &gUnk_030053F4, &gUnk_030053F0) && !Tst32(4, &gUnk_02000560))
                    SceneAnimation_InitTimedCollider(t);
                else {
                    Nidhoggr_UnregisterTerrainCells(t);
                    t->state++;
                }
            }
            break;
        case 8:
            break;
    }
}
