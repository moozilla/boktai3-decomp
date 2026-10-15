#ifndef DAMAGE_CONTROLLER_H
#define DAMAGE_CONTROLLER_H
#include "global.h"

// An invisible script-placed hit box: accumulates damage from colliders matching a kind/flag mask,
// runs a script on hit and on death, then frees itself or refills its health.

struct DamageVec {
    s16 x, y, z, pad;
};

struct DamagePackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

// Same layout as struct SceneCollider, with the fields this unit uses named.
struct DamageCollider {
    u8 pad0[6];
    u16 state; // bit 2: collision disabled
    u8 pad8[4];
    struct DamageVec position;
    u8 pad14[32];
    u32 kind, flags; // what the colliding thing is (attack kind bits)
    u16 pad3c, damage;
    u8 pad40[20];
};

struct DamageController {
    u8 task[24];
    struct DamageCollider collider;
    u16 id;
    s16 health;
    u16 maximum, flags; // flags: 1 free on death, 2/4 exact kind/flag match, 8/16 match any
    u32 kindMask, flagMask;
    u32 height; // collider parameter (script 'S')
    u16 cooldown, delay, hitScript, deathScript;
    struct DamageVec hitPosition;
};

void DamageController_AccumulateHit(struct DamageCollider *attack, struct DamageCollider *target,
                                    struct DamageController *controller);
s32 DamageController_Update(struct DamageController *);
u32 sub_0800C954(struct DamageController *);
s32 DamageController_Initialize(struct DamageController *, u32 id);
void *sub_0800CB60(u32);

/* Engine helpers. */
void Script_Call(u32, void *);
void Task_Kill(void *);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Collider_Init(void *, u32, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF7C(void *, u32, u32, u32);
s32 Collider_Refresh(void *);
void Collider_Remove(void *);
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);

#endif
