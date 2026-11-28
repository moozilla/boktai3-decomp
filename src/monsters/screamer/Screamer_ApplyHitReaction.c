#include "global.h"
#include "engine/math/hit_spark_random.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "monsters/screamer/screamer_state.h"
struct Vec {
    u16 x, y, z, pad;
};
extern const struct Vec gUnk_0824FE9C;
void Collision_CalculateDamageResponse(void *, void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Screamer_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                               struct ScreamerState *screamer)
{
    struct {
        struct Vec position, velocity;
    } frame;

    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        screamer->f1DC = hit->cost;
        screamer->f204 = source->contactFlags;
        screamer->bodyCollider.responseValue = source->responseValue;
        if (source->reactionFlags & 0x40) {
            screamer->bodyCollider.responseValue = 0x26;
            screamer->f1DE = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&screamer->bodyCollider, screamer->f1DC, 0,
                                     &screamer->bodyCollider.position,
                                     &screamer->bodyCollider.offset, 1);
            SceneHud_SetTimedState(screamer->f1F8, screamer->f1E4, screamer->f1E6, screamer->f1EC,
                                   screamer->bodyCollider.costFlags == 4, screamer->f1F4);
            if (source->reactionFlags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->reactionFlags & (u16)screamer->f1EA) {
            s32 i;
            struct Vec *vel = &frame.velocity;
            struct Vec *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            u32 mask = 0x7f;
            for (i = 2; i >= 0; i--) {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE9C;
                frame.position = *(struct Vec *)&screamer->sprite.position;
                value = (u32)gRandomTable;
                random = HitSpark_NextIndex(*index, limit);
                sample = HitSpark_Sample(value, random);
                x = pos->x - 0x40;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = HitSpark_NextIndex(random, limit);
                pos->y += (u8)HitSpark_Sample(value, random);
                random = HitSpark_NextIndex(random, limit);
                *index = random;
                sample = HitSpark_Sample(value, random);
                x = pos->z - 0x40;
                sample &= mask;
                x += sample;
                pos->z = x;
                value = 0;
                SparkParticle_Spawn(&frame.position, 1, vel, value, 3, 8, 2, 2);
            }
        }
    }
}
