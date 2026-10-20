#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Point {
    u32 active;
    struct Vec position;
    u8 pad[12];
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0x944];
    struct Point points[9], targets[3];
    u8 padE38[0x14];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
#define Ratatosk_UpdateSunOrbSummon sub_081F48AC
s32 Ratatosk_FindPointNearestReference(void *, s32);
void RisingActorConfig_Initialize(void *);
void SunOrb_SetTargetSource(void *, s32);
s32 SunOrb_CreateAtPosition(void *, void *);
void RisingActorManager_BeginRaise(s32);
void Ratatosk_UpdateSunOrbSummon(struct Actor *p, s32 tick)
{
    u8 config[0x20];
    struct Actor *p2 = p;
    if (p->changed) {
        u32 zero = 0;
        p->changed = zero;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 15, 1, 4);
        p->phase = zero;
        p->selectedPoint = 0xffff;
        p->selectedTarget = -1;
        Sound_PlaySong(0x570);
    }
    switch (p->phase) {
        case 0:
            if (tick > 5) {
                SceneActors_StartUnmirroredAnimation(p, p2->animationData, p2->animation, 16, 1, 4);
                p2->phase++;
            }
            break;
        case 1:
            if (tick > 85) {
                SceneActors_StartUnmirroredAnimation(p, p2->animationData, p2->animation, 15, 1, 4);
                p2->phase++;
                Sound_PlaySong(0x55b);
            }
            break;
        case 2: {
            s32 index = Ratatosk_FindPointNearestReference(p2, 0);
            s32 slot;
            if (index < 0) {
                p2->phase = 4;
                break;
            }
            RisingActorConfig_Initialize(config);
            config[7] = index;
            SunOrb_SetTargetSource(config, 1);
            slot = SunOrb_CreateAtPosition(config, &p2->points[index].position);
            if (slot >= 0) {
                p2->points[index].active = 1;
                p2->targets[slot].active = 1;
                RisingActorManager_BeginRaise(slot);
                p2->selectedPoint = slot;
            }
            SceneActors_StartUnmirroredAnimation(p, p2->animationData, p2->animation, 16, 1, 4);
            p2->phase++;
            p->finished = 1;
        } break;
        case 3:
        case 4:
            p->finished = 1;
            break;
    }
    Callback_InvokeActor(p, p->callback);
}
