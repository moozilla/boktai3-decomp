#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "player_cue_sequence.h"
#include "monster_particle_random.h"
#define FrostEmitter_SpawnParticle sub_081D2D78
struct FrostParticle {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    u16 lifetime, unknown46;
};
struct FrostEmitter {
    u8 unknown0[0x18];
    u8 graphics[0x1C];
    void *resource;
    struct ContactPosition *source;
    u32 emitting;
    struct FrostParticle particles[12];
    u32 enabled, age;
};
extern const s16 gUnk_086149C4[];

static inline s32 ShiftRandom(u16 value, s32 shift)
{
    return value >> shift;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}

void FrostEmitter_SpawnParticle(struct FrostEmitter *input)
{
    struct FrostEmitter *p = input;
    struct FrostParticle *slot = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!p->particles[i].lifetime) {
            slot = &p->particles[i];
            break;
        }
    }
    if (slot) {
        u32 sampleOrigin;
        const u16 *table;
        u32 index;
        u32 secondIndex;
        s32 angle;
        s32 magnitude;
        s32 mask;
        s32 sample;
        struct ContactPosition *velocity;
        struct FrostEmitter *animationOwner;
        slot->sprite.position = *p->source;
        PARTICLE_RANDOM_CAPTURE_ORIGIN(sampleOrigin);
        table = gRandomTable;
        index = (gRandomIndex + 1) & 1023;
        angle = ShiftRandom(table[index], 4);
        mask = 63;
        animationOwner = p;
        secondIndex = (index + 1) & 1023;
        gRandomIndex = secondIndex;
        sample = ShiftRandom(table[secondIndex], 4) & 1;
        velocity = &slot->velocity;
        angle &= mask;
        magnitude = sample + 1;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * magnitude);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * magnitude);
        {
            s32 lift = ShiftRandom(
                ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR), 4);
            slot->velocity.y = (lift & 3) + 4;
        }
        slot->lifetime = 60;
        Anim_SetSequence(&slot->sequence, animationOwner->resource, 0, 0, 4);
        slot->sprite.flags &= ~1;
    }
}
