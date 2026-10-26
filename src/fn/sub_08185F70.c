#include "global.h"
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
extern struct ArmorTrackingTarget *gGameState;
s32 Math_Atan2(s32, s32);
void ActorController_StartTurn(void *, u32);
void ActorController_StartModeTwoTurn(void *, u32);
void ActorController_StartModeThreeTurn(void *, u32);
s32 DirectionalActor_TryBeginState(void *, s32);
// confidence=low: computes an offset around the player before selecting movement or a turn.
void Armor_UpdateOffsetTracking(struct ArmorTrackingActor *p, s32 mode, s32 alternate)
{
    s32 dx = gGameState->x - p->x;
    s32 dz = gGameState->z - p->z;
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
                goal.x = gGameState->x + distance;
                goal.z = gGameState->z;
            } else if (sector <= 63) {
                goal.x = gGameState->x + diagonal;
                goal.z = gGameState->z + diagonal;
            } else if (sector <= 95) {
                goal.x = gGameState->x;
                goal.z = gGameState->z + distance;
            } else if (sector <= 127) {
                goal.x = gGameState->x - diagonal;
                goal.z = gGameState->z + diagonal;
            } else if (sector <= 159) {
                goal.x = gGameState->x - distance;
                goal.z = gGameState->z;
            } else if (sector <= 191) {
                goal.x = gGameState->x - diagonal;
                goal.z = gGameState->z - diagonal;
            } else if (sector <= 223) {
                goal.x = gGameState->x;
                goal.z = gGameState->z - distance;
            } else {
                goal.x = gGameState->x + diagonal;
                goal.z = gGameState->z - diagonal;
            }
        } else {
            sector = (heading + 7) & 255;
            if (sector <= 31) {
                goal.x = gGameState->x + diagonal;
                goal.z = gGameState->z + diagonal;
            } else if (sector <= 63) {
                goal.x = gGameState->x;
                goal.z = gGameState->z + distance;
            } else if (sector <= 95) {
                goal.x = gGameState->x - diagonal;
                goal.z = gGameState->z + diagonal;
            } else if (sector <= 127) {
                goal.x = gGameState->x - distance;
                goal.z = gGameState->z;
            } else if (sector <= 159) {
                goal.x = gGameState->x - diagonal;
                goal.z = gGameState->z - diagonal;
            } else if (sector <= 191) {
                goal.x = gGameState->x;
                goal.z = gGameState->z - distance;
            } else if (sector <= 223) {
                goal.x = gGameState->x + diagonal;
                goal.z = gGameState->z - diagonal;
            } else {
                goal.x = gGameState->x + distance;
                goal.z = gGameState->z;
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
