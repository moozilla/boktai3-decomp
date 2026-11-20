#ifndef GUARD_PHASE_OBJECT_H
#define GUARD_PHASE_OBJECT_H

#include "contact_collider.h"
#include "contact_position.h"
#include "global.h"

struct PhaseTarget {
    u8 unknown0[0x30];
    struct ContactPosition position;
};

struct PhaseEntryPresentation {
    s8 opacity;
    u8 unknown9[0x13];
    s16 x, y, z, unknown22;
    u8 unknown24[8];
};

struct PhaseEntry {
    u32 flags;
    u8 unknown4[2];
    u8 phase;
    u8 unknown7;
    struct PhaseEntryPresentation presentation;
};

struct PhaseObjectMode {
    u8 flags;
    u8 unknown8E[4];
    u8 active, mode;
} __attribute__((packed));

/* Shared fields of the four-entry phase actor. Several state roles are tentative. */
struct PhaseObject {
    u8 unknown0[0x60];
    struct PhaseTarget *target;
    s16 offsetX, unknown66, offsetZ, unknown6A;
    u16 scatterStepX, unknown6E, scatterStepZ;
    u8 unknown72[0x1B];
    struct PhaseObjectMode groupMode;
    u8 unknown94[0x14];
    struct PhaseEntry entries[4];
    u8 unknown158[0x70];
    struct ContactCollider memberCollidersA[4], memberCollidersB[4];
    struct ContactPosition saved[4], goal, delta, start;
    u8 unknown4A0[0x10];
    s8 turnSpeed;
    u8 targetAngle;
    u8 unknown4B2[2];
    u16 turnFrames;
    u8 unknown4B6[2];
    s32 countdown;
    u8 unknown4BC[0xB54 - 0x4BC];
    u16 countdownStep;
    u8 unknownB56[0xB68 - 0xB56];
    u32 drawableFlags;
    u8 unknownB6C[2];
    u8 rotation;
    u8 unknownB6F[0xB84 - 0xB6F];
    struct ContactPosition position;
    u8 unknownB8C[0xC1C - 0xB8C];
    struct ContactCollider colliderA, colliderB;
    u8 unknownCC4[4];
    void (*update)(struct PhaseObject *);
    u8 unknownCCC[0xC];
    void (*transition)(struct PhaseObject *);
    void (*secondaryTransition)(struct PhaseObject *);
    void (*scatterTransition)(struct PhaseObject *);
    u8 unknownCE4[0x10];
    s32 scatterDelay;
    u8 unknownCF8[0xD5A - 0xCF8];
    s16 rotationTimer;
    u8 unknownD5C[0xD80 - 0xD5C];
    u32 frame;
};

/* Changing group mode queues its entry action. The active byte is the
 * enable value that the other group states test before dispatch.
 */
static inline void PhaseObject_EnterMode(struct PhaseObject *object, u8 active, u32 mode)
{
    object->groupMode.active = active;
    object->groupMode.mode = mode;
    object->groupMode.flags = active | object->groupMode.flags;
}

#endif
