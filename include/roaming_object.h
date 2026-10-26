#ifndef GUARD_ROAMING_OBJECT_H
#define GUARD_ROAMING_OBJECT_H
#include "contact_swarm.h"
/* Two neighboring 0x144-byte actors share this observed layout. Identity is tentative. */
struct RoamingObject {
    u8 task[0x18];
    u16 id;
    u8 unknown1A[0x25 - 0x1A];
    u8 angle;
    u8 unknown26[2];
    struct ContactPosition position;
    struct ContactPosition velocity;
    u8 unknown38[0x64 - 0x38];
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct ContactPosition home;
    u8 unknownB4[8];
    struct ContactCollider collider;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    struct {
        u32 unknown0, flags, unknown8[2];
    } listener;
    u16 targetAngle, turnTimer, animation, impulse, state, flashCount;
    void (*update)(struct RoamingObject *);
};
extern const s16 gUnk_086149C4[];
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
void RoamingObject_SetState(struct RoamingObject *, u16);
void RoamingObjectB_SetState(struct RoamingObject *, u16);
#endif
