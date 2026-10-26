// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
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
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 done;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sp->flags |= 4;
        else
            sp->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sp->flags |= 8;
        else
            sp->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    done = 1;
                } else {
                    state->index = index - 1;
                    done = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    done = 1;
                } else {
                    done = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            done = 0;
        }
    }
    return done;
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
            register u32 offset asm("r1");
            sub_0810638C(p);
            offset = 0x10E;
            asm("" : "+r"(offset));
            {
                u16 *frames = (void *)((u8 *)p + offset);
                register u32 zero asm("r1") = 0;
                asm("" : "+r"(zero));
                *frames = 0;
                p->entered = zero;
            }
        }
        done = Step(&p->sprite, &p->sequence);
        if (p->frames <= 31) {
            union SparkPosition position;
            if (Mod(p->frames, 6) == 0) {
                position.position = p->origin;
                position.packed.x -= 64;
                {
                    register union SparkPosition *output asm("r1") = &position;
                    asm("" : "+r"(output));
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
