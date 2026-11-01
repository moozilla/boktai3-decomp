// MATCHING_CONCESSION: register-pinning
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
union SparkPosition {
    struct ContactPosition position;
    struct {
        u32 x : 16, y : 16, z : 16, pad : 16;
    } packed;
};
// confidence=low: emits periodic sparkles around the origin while the transition animation plays.
void RegionActor_AnimateSparkleTransition(struct RegionSprite *p)
{
    struct RegionSpriteManager *manager = gRegionActorPool;
    u32 done;
    if (manager) {
        if (p->entered) {
            u32 offset;
            sub_0810638C(p);
            offset = 0x10E;
            {
                u16 *frames = (void *)((u8 *)p + offset);
                u32 zero = 0;
                asm("" : "+r"(zero));
                *frames = 0;
                p->entered = zero;
            }
        }
        done = PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
        if (p->frames <= 31) {
            union SparkPosition position;
            if (Mod(p->frames, 6) == 0) {
                position.position = p->origin;
                position.packed.x -= 64;
                {
                    union SparkPosition *output = &position;
                    AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, output);
                }
            }
            if (Mod(p->frames, 10) == 0) {
                position.position = p->origin;
                position.position.x += 64;
                AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, &position);
            }
            if (Mod(p->frames, 16) == 0) {
                position.position = p->origin;
                position.position.z += 32;
                AnimatedParticles_EmitChargeSparkle(1, 0, 0, 64, &position);
            }
            if (Mod(p->frames, 7) == 0) {
                s8 *sound = (s8 *)manager + 0xA4;
                if (*sound < 0) {
                    Sound_PlaySong(0x15C);
                    *sound = 1;
                }
            }
            p->frames++;
        } else if (done)
            p->kind = 0;
    }
}
