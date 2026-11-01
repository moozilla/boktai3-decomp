#include "global.h"
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
// confidence=low: updates animation, enters the death state on depleted health, otherwise restores ten health.
void RegionActor_UpdateHealthAnimation(struct RegionSprite *p)
{
    struct ContactSwarmSprite *sprite = &p->sprite;
    sprite->flags &= ~1;
    if (gRegionActorPool) {
        PlayerCue_StepSequenceEnded(sprite, &p->sequence);
        if (p->hit & 1) {
            if (p->health <= 0) {
                p->behaviorFlags = 0x28;
                p->kind = 4;
                p->entered = 1;
            } else
                p->hit = 0;
        } else if (p->health < p->maxHealth)
            p->health += 10;
    }
}
