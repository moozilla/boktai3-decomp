#include "player_cue_sequence.h"
#include "ram.h"
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
};
struct RegionSpriteManager {
    u8 unknown0[0x18];
    const void *sequenceResource, *alternateResource;
};
void ProximityActor_UpdateDamageAnimation(struct RegionSprite *);
static inline u8 Test(struct RegionSprite *actor, u32 bit)
{
    if (actor->behaviorFlags & bit)
        return 1;
    return 0;
}
void RegionActor_UpdateBehaviorAnimation(struct RegionSprite *input)
{
    struct RegionSprite *p = input;
    struct ContactSwarmSprite *sprite = &input->sprite;
    struct RegionSpriteManager *manager = gRegionActorPool;
    if (manager) {
        p->sprite.flags &= ~1;
        if (Test(p, 0x10)) {
            if (PlayerCue_StepSequenceEnded(sprite, &p->sequence)) {
                Anim_SetSequence(&p->sequence, manager->sequenceResource, 0, 0, 0);
                p->sequence.index = 3;
                p->behaviorFlags = 0x21;
            }
        } else if (Test(p, 2)) {
            PlayerCue_StepSequence(sprite, &p->sequence);
            if (p->sequence.index == 3)
                p->behaviorFlags = 0x24;
        } else {
            ProximityActor_UpdateDamageAnimation(p);
            if (Test(p, 1))
                PlayerCue_StepSequence(&p->sprite, &p->sequence);
        }
    }
}
