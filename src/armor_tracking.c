#include "global.h"
#include "armor_action.h"

s32 DirectionalActor_TryBeginState(void *, s32);
void ActorController_UpdateTargetMovement(void *, s32, s32);
extern void *gGameState;

struct S {
    u8 f0[0x48];
    s16 a48;
    s16 a4a;
    s16 a4c;
    u8 g0[0xB3A - 0x4E];
    u8 b3a;
    u8 f1;
    u8 b3c;
    u8 b3d;
    u8 b3e;
    u8 b3f;
    u8 b40;
    u8 f2[4];
    u8 b45;
    u8 g1[0x1124 - 0xB46];
    s16 x;
    s16 y;
    s16 z;
};

void ActorController_ForceAction(struct S *, s32);

void Armor_FinalizeDefeat(struct S *);

void Armor_UpdateAlternateDeathSequence(struct S *p)
{
    switch (p->b3d) {
        case 0:
            if (p->b3f != 0x2e)
                ActorController_ForceAction(p, 0x2e);
            p->b3d = 1;
            break;
        case 1:
            if (p->b40 == 0xff) {
                p->b3d = 0xff;
                Armor_FinalizeDefeat(p);
            }
            break;
    }
}


void Armor_UpdateAction30(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (DirectionalActor_TryBeginState(p, 0x1e) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

struct Armor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad4E[0xb32 - 0x4e];
    u16 flags;
    u8 padB34[5];
    u8 direction;
    u8 padB3A[3];
    u8 state;
};

s32 Math_Atan2(s32, s32);

u32 ActorController_StartAlternateTurn(struct Armor *, u32);


void Armor_UpdateFacingAction(struct Armor *p, s32 alternate)
{
    switch (p->state) {
        case 0: {
            u8 *target = gGameState;
            s32 direction =
                (Math_Atan2(*(s16 *)(target + 0x30) - p->x, *(s16 *)(target + 0x34) - p->z) + 32) &
                0xc0;
            if (p->direction != direction) {
                if (ActorController_StartAlternateTurn(p, direction))
                    p->state = 1;
                break;
            }
            p->state = 1;
        }
        case 1:
            if (alternate) {
                if (DirectionalActor_TryBeginState(p, 0x20))
                    p->state = 2;
            } else {
                if (DirectionalActor_TryBeginState(p, 0x19))
                    p->state = 2;
            }
            break;
        case 2:
            if (p->flags & 0x10)
                p->state = 255;
            break;
    }
}


void Armor_UpdateFacingAction25(u8 *p)
{
    Armor_UpdateFacingAction(p, 0);
}


void Armor_UpdateFacingAction32(s32 a)
{
    Armor_UpdateFacingAction(a, 1);
}


void Armor_UpdateTrackingAction31(s32 a)
{
    ActorController_UpdateTargetMovement(a, 0, 1);
}

void Armor_UpdateTrackingAction44(s32 a)
{
    ActorController_UpdateTargetMovement(a, 2, 2);
}

struct ContactPackedVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

struct ArmorTrackingActor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad4e[0xb32 - 0x4e];
    u16 flags;
    u8 padb34[5];
    u8 facing, padb3a, direction, behavior, phase, trackingFlags;
    u8 padb3f[2];
    u8 ready;
    u8 padb42[3];
    u8 delay;
};

struct ArmorTrackingTarget {
    u8 pad[0x30];
    s16 x, y, z;
};


void ActorController_StartTurn(void *, u32);

void ActorController_StartModeTwoTurn(void *, u32);

void ActorController_StartModeThreeTurn(void *, u32);

// confidence=low: computes an offset around the player before selecting movement or a turn.

void Armor_UpdateOffsetTracking(struct ArmorTrackingActor *p, s32 mode, s32 alternate)
{
    s32 dx = ((struct ArmorTrackingTarget *)gGameState)->x - p->x;
    s32 dz = ((struct ArmorTrackingTarget *)gGameState)->z - p->z;
    s32 heading = Math_Atan2(dx, dz);
    if (p->flags & 0x10)
        p->delay = 120;
    if (!(p->trackingFlags & 3)) {
        struct ContactPackedVector goal;
        s32 distance, diagonal, ax, az, sector;
        goal.y = p->y;
        ax = dx;
        az = dz;
        if (dx < 0)
            ax = -dx;
        if (dz < 0)
            az = -dz;
        if (ax + az <= 512) {
            distance = 3072;
            diagonal = 2172;
        } else {
            distance = 768;
            diagonal = 543;
        }
        if (p->phase == 1) {
            sector = (heading - 7) & 255;
            if (sector <= 31) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + distance;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z;
            } else if (sector <= 63) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + diagonal;
            } else if (sector <= 95) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + distance;
            } else if (sector <= 127) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + diagonal;
            } else if (sector <= 159) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - distance;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z;
            } else if (sector <= 191) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - diagonal;
            } else if (sector <= 223) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - distance;
            } else {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - diagonal;
            }
        } else {
            sector = (heading + 7) & 255;
            if (sector <= 31) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + diagonal;
            } else if (sector <= 63) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + distance;
            } else if (sector <= 95) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z + diagonal;
            } else if (sector <= 127) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - distance;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z;
            } else if (sector <= 159) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x - diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - diagonal;
            } else if (sector <= 191) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - distance;
            } else if (sector <= 223) {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + diagonal;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z - diagonal;
            } else {
                goal.x = ((struct ArmorTrackingTarget *)gGameState)->x + distance;
                goal.z = ((struct ArmorTrackingTarget *)gGameState)->z;
            }
        }
        dx = goal.x - p->x;
        dz = goal.z - p->z;
        p->direction = (Math_Atan2(dx, dz) + 16) & -32;
    }
    if (p->ready) {
        if ((u32)(((heading - p->facing) & 255) - 49) <= 158) {
            heading = (heading + 32) & 192;
            switch (mode) {
                case 0:
                    ActorController_StartTurn(p, heading);
                    break;
                case 1:
                    ActorController_StartModeTwoTurn(p, heading);
                    break;
                case 2:
                    ActorController_StartModeThreeTurn(p, heading);
                    break;
            }
        } else {
            switch (mode) {
                case 0:
                    if (alternate)
                        DirectionalActor_TryBeginState(p, 35);
                    else
                        DirectionalActor_TryBeginState(p, 34);
                    break;
                case 1:
                    DirectionalActor_TryBeginState(p, 37);
                    break;
                case 2:
                    DirectionalActor_TryBeginState(p, 41);
                    break;
            }
        }
    }
}

struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};



void Armor_UpdateDelayedTrackingModeZero(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        ActorController_UpdateTargetMovement(p, 0, 0);
    } else {
        Armor_UpdateOffsetTracking(p, 0, 0);
    }
}

void Armor_UpdateDelayedTrackingAction31(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        ActorController_UpdateTargetMovement(p, 0, 1);
    } else {
        Armor_UpdateOffsetTracking(p, 0, 1);
    }
}

void Armor_UpdateDelayedTrackingModeTwo(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        ActorController_UpdateTargetMovement(p, 2, 0);
    } else {
        Armor_UpdateOffsetTracking(p, 1, 0);
    }
}

void Armor_UpdateDelayedTrackingModeThree(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        ActorController_UpdateTargetMovement(p, 3, 0);
    } else {
        Armor_UpdateOffsetTracking(p, 2, 0);
    }
}


void Armor_StopAndFinishBehavior(struct E *p)
{
    DirectionalActor_TryBeginState(p, 0);
    p->a = 0xff;
}

void Armor_UpdateAction36Completion(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (DirectionalActor_TryBeginState(p, 0x24) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}
