#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct RegionSprite {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x90 - 0x2C];
    struct DisplaySequenceState sequence;
    struct ContactPosition origin;
    u8 unknownA8[0xEB - 0xA8];
    u8 hit;
    u8 unknownEC[0xFC - 0xEC];
    s16 health, maxHealth;
    s32 threshold;
    s16 resourceId, kind;
    u32 behaviorFlags;
    u8 entered, mode;
    u16 frames;
    u8 overlayActive, variant, unknown112[2];
    s32 region;
    u8 terrainCell[0x10];
};
struct RegionSpriteManager {
    u8 unknown0[0x18];
    const void *sequenceResource, *alternateResource;
};
void ProximityActor_UpdateDamageAnimation(struct RegionSprite *);
void sub_0810638C(struct RegionSprite *);
static inline u8 Test(u32 *flags, u32 bit)
{
    if (*flags & bit)
        return 1;
    return 0;
}
void AnimatedParticles_EmitChargeSparkle(s32, s32, s32, s32, void *);
// confidence=low: entered flag selects the final alternate animation and emits charge sparkles.
void RegionActor_AnimateRemoval(struct RegionSprite *p)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    if (manager) {
        if (p->entered) {
            sub_0810638C(p);
            Anim_SetSequence(&p->sequence, manager->alternateResource, 4, 0, 0);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            p->frames = 0;
            p->entered = 0;
            AnimatedParticles_EmitChargeSparkle(1, 2, 0, 64, &p->sprite.position);
        }
        if (PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence)) {
            p->kind = 0;
            p->sprite.flags |= 1;
            AnimatedParticles_EmitChargeSparkle(3, 2, 0, 64, &p->sprite.position);
        }
    }
}
