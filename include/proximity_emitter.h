#ifndef GUARD_PROXIMITY_EMITTER_H
#define GUARD_PROXIMITY_EMITTER_H
#include "contact_swarm.h"
/* Provisional: paired sprites and eight rising particles near the tether actor. */
struct ProximityEmitterParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct ContactPosition position;
    u8 unknown20[8];
    s16 velocityX, velocityY, velocityZ;
    u16 unknown2E, active, frame;
};
struct ProximityEmitter {
    u8 task[0x18];
    struct ContactSwarmDrawPacket primary, secondary;
    u16 nearby, stateFrame;
    struct ContactPosition position;
    void (*update)(struct ProximityEmitter *);
    void *particleResource;
    u8 unknownBC, nextParticle;
    u16 particleFrame;
    struct ProximityEmitterParticle particles[8];
    u32 enterScriptId, leaveScriptId;
};
struct ProximityEmitterPlayer {
    u8 unknown0[0x255];
    u8 activation;
};
extern struct ProximityEmitterPlayer *gUnk_02000488;
struct ContactPosition *TetherActor_GetPosition(void);
void TetherActor_RequestPaletteBlend(u32);
void sub_08217EEC(void *, void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08217ECC(void *, u32);
void sub_08217DE0(void *, void *, u32);
void sub_08051E10(struct ProximityEmitter *);
void sub_08051D28(struct ProximityEmitter *);
u8 *sub_0821D64C(s32, s32);
void sub_08051A8C(struct ProximityEmitter *, void *);
u32 sub_08051B70(struct ProximityEmitter *);
void sub_08051BD8(struct ProximityEmitter *);
void sub_08051CB4(struct ProximityEmitter *);
void sub_08051A3C(struct ProximityEmitterParticle *, void *);
s32 sub_08051DA4(struct ProximityEmitter *);
u32 sub_08051DE4(struct ProximityEmitter *);
s32 sub_08051EA8(struct ProximityEmitter *, u32, u32);
#endif
