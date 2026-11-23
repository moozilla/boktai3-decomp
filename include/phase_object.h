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
    u8 unknown72[2];
    s16 driftX, unknown76, driftZ; /* added to the anchor approach, halved each frame */
    u8 unknown7A[4];
    s16 health;
    u8 unknown80[4];
    s16 contactReward; /* passed to PlayerStats_AddContactHundredths on a hit */
    u8 unknown86[7];
    struct PhaseObjectMode groupMode;
    u8 unknown94[8];
    u16 flashTimer;
    u8 unknown9E[4];
    u8 entryReset;
    u8 unknownA3[5];
    struct PhaseEntry entries[4];
    u8 unknown158[0x70];
    struct ContactCollider memberCollidersA[4], memberCollidersB[4];
    struct ContactPosition saved[4], goal, delta, start;
    s32 orbitAngles[4];
    s8 turnSpeed;
    u8 targetAngle;
    u8 orbitAngle; /* the member colliders orbit at this angle (28 - rotation) */
    u8 unknown4B3;
    u16 turnFrames;
    u8 unknown4B6[2];
    s32 countdown;
    u8 unknown4BC[0xB4C - 0x4BC];
    u16 launchInterval, launchDelay;
    u16 memberCount, countdownLimit;
    u16 countdownStep;
    u8 unknownB56[0xB68 - 0xB56];
    u32 drawableFlags;
    u8 unknownB6C[2];
    u8 rotation;
    u8 unknownB6F[5];
    void *damageAnimation;
    u8 unknownB78[0xC];
    struct ContactPosition position;
    u8 unknownB8C[0xC1C - 0xB8C];
    struct ContactCollider colliderA, colliderB;
    s8 red, green, blue; /* the color pulse's current offsets */
    u8 unknownCC7;
    void (*update)(struct PhaseObject *);
    u8 unknownCCC[8];
    void (*retarget)(struct PhaseObject *);
    void (*transition)(struct PhaseObject *);
    void (*secondaryTransition)(struct PhaseObject *);
    void (*scatterTransition)(struct PhaseObject *);
    void (*launchUpdate)(struct PhaseObject *);
    void (*attackUpdate)(struct PhaseObject *);
    void (*hitUpdate)(struct PhaseObject *); /* the hit flash, set on a damaging contact */
    void (*phaseUpdate)(struct PhaseObject *);
    s32 scatterDelay;
    u32 bobbingTimer;
    s32 pulseTimer; /* frames into the color pulse step */
    u32 landingHeight;
    u8 unknownD04[0x24];
    u16 launchFrames, attackFrames;
    u8 launchCount, attackHeading;
    u8 unknownD2E[4];
    u16 color; /* the palette color the pulse fades from */
    u8 unknownD34[0x1E];
    s8 pulseDuration;
    s8 actionCompletions;
    u8 pulseMode;
    u8 attackMode;
    u8 unknownD56[2];
    u16 damagePalette;
    s16 rotationTimer;
    u8 unknownD5C[4];
    u32 hitResponseD60, hitResponseD64, hitResponseD68;
    u8 unknownD6C[0xC];
    u16 damage, damageKind;
    u8 unknownD7C[4];
    u32 frame;
    u16 orbitFrames;
    u8 unknownD86[0x12];
    struct ContactPosition previousPosition;
    s32 hitFrames;
};

static inline void PhaseObject_SetFlags(u8 bits, struct PhaseObject *object)
{
    object->groupMode.flags = bits | object->groupMode.flags;
}

/* Changing group mode queues its entry action. The active byte is the
 * enable value that the other group states test before dispatch.
 */
static inline void PhaseObject_EnterMode(struct PhaseObject *object, u8 active, u32 mode)
{
    object->groupMode.active = active;
    object->groupMode.mode = mode;
    PhaseObject_SetFlags(active, object);
}

static inline u8 PhaseObject_TakeEntry(struct PhaseObject *object, u32 mask)
{
    u32 flags = object->groupMode.flags;
    if (mask & flags) {
        u32 remaining = ~mask;
        remaining &= flags;
        object->groupMode.flags = remaining;
        object->entryReset = 0;
        return 1;
    }
    return 0;
}

#endif
