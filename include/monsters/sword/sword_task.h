#ifndef GUARD_SWORD_TASK_H
#define GUARD_SWORD_TASK_H

#include "scene/collision/contact_swarm.h"

struct SwordTask;

struct SwordTaskStatus {
    u8 registry[4];
    u32 flags;
};

struct SwordComponent {
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider attack;
    struct ContactCollider body;
    u16 fieldF0;
    u16 fieldF2;
    u16 fieldF4;
    u8 padF6[0x2];
    u16 fieldF8;
    u16 fieldFA;
    u16 fieldFC;
    u8 padFE[0xC];
    u16 field10A;
    u8 pad10C[0xC];
    u8 index, entryFlags, angle;
    u8 pad11B[0x3];
    u8 active, phase;
    u8 pad120[0x6];
    u16 field126;
    u16 field128;
    u8 field12A;
    u8 field12B;
    u8 pad12C[2];
    u8 entryReset;
    u8 pad12F[0x31];
    struct SwordTask *parent;
    void (*update)(struct SwordComponent *);
    u32 field168;
    u8 pad16C[4];
    void (*bobUpdate)(struct SwordComponent *);
    void (*waveUpdate)(struct SwordComponent *);
    void (*thrustUpdate)(struct SwordComponent *);
    void (*lungeUpdate)(struct SwordComponent *);
    u32 field180;
    u32 field184;
    u8 pad188[2];
    s8 orbitDirection, turnSteps;
    u8 pad18C[8];
};

/* Sword_Create allocates one task and Sword_UpdateComponents dispatches these four children. */
struct SwordTask {
    u8 unknown0[0x60];
    void *target;
    u16 field64, field66, field68, field6A, field6C, field6E, field70;
    u8 unknown72[0xC];
    s16 health, body80, field82, field84, field86, body88, body8A;
    u8 field8C, flags8D, field8E, field8F, field90, field91, field92, field93;
    u8 unknown94[0x6];
    u16 field9A, field9C;
    u8 field9E, field9F;
    u8 unknownA0[4];
    u16 valueA4;
    u8 unknownA6[2];
    struct ContactSwarmSprite segments[4];
    struct ContactSwarmGraphics segmentGraphics[4];
    struct ContactCollider segmentAttacks[4];
    struct ContactCollider segmentBodies[4];
    u8 unknown468[0x54];
    struct SwordComponent components[4];
    u8 unknownB0C[0x4C];
    struct DisplaySequenceState sequence;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    u8 shadow[0x6C];
    struct ContactCollider attack;
    struct ContactCollider body;
    u8 unknownCC4[0x4];
    void *update;
    u8 unknownCCC[0x4];
    void *onContact;
    u8 unknownCD4[0x18];
    u32 fieldCEC;
    void *colorUpdate;
    u8 unknownCF4[0x4];
    u32 duration;
    u8 unknownCFC[0x8];
    s16 attackD04[2], attackD08[2], attackD0C[2], attackD10[2];
    s16 fieldD14, bodyD16, fieldD18, fieldD1A, bodyD1C, bodyD1E;
    s16 attackD20, attackD22, attackD24, attackD26;
    u8 unknownD28[4];
    u8 count, angle, unknownD2E[2];
    u16 palette[16];
    u8 unknownD50[0x2];
    u8 fieldD52, fieldD53, fieldD54, variant;
    u16 paletteBank;
    u16 animation;
    u8 unknownD5A[2];
    u16 initialHealth;
    u8 parameter;
    u8 unknownD5F[0xD];
    s32 valueD6C, arguments[2];
    u16 damage, damageKind, objectId;
    u8 fieldD7E, unknownD7F;
    s32 ticks;
    u8 unknownD84[2];
    u16 fieldD86;
    struct SwordTaskStatus status;
    u8 unknownD90[0x14];
};

typedef char SwordComponentSizeCheck[sizeof(struct SwordComponent) == 0x194 ? 1 : -1];
typedef char SwordTaskSizeCheck[sizeof(struct SwordTask) == 0xDA4 ? 1 : -1];

#endif
