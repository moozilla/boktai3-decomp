#ifndef GUARD_PROXIMITY_EMITTER_H
#define GUARD_PROXIMITY_EMITTER_H
#include "contact_swarm.h"
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
extern struct ProximityEmitterPlayer *gTetherActor;
struct ContactPosition *TetherActor_GetPosition(void);
void TetherActor_RequestPaletteBlend(u32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
void ParticleSprite_Initialize(void *, void *, u32);
void ProximityEmitter_InitializeParticles(struct ProximityEmitter *);
void ProximityEmitter_TickGrow(struct ProximityEmitter *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ProximityEmitter_EmitParticle(struct ProximityEmitter *, void *);
u32 ProximityEmitter_IsTetherNearby(struct ProximityEmitter *);
void ProximityEmitter_TickActive(struct ProximityEmitter *);
void ProximityEmitter_TickRise(struct ProximityEmitter *);
void ProximityEmitter_TickParticle(struct ProximityEmitterParticle *, void *);
s32 ProximityEmitter_Tick(struct ProximityEmitter *);
u32 ProximityEmitter_Destroy(struct ProximityEmitter *);
s32 ProximityEmitter_InitializeFromScript(struct ProximityEmitter *, u32, u32);
#endif
