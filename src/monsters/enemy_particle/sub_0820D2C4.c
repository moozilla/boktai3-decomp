#include "engine/math/math_shift.h"
#include "engine/engine_pause.h"
#include "monsters/enemy_particle.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence_sprite.h"
#include "player/player.h"
extern const s16 gUnk_086149C4[];
void EnemyParticle_ReleaseSlot(struct EnemyParticlePool *, struct EnemyParticle *, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline u8 *cell(s32 i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline void ComputeVelocity(struct ContactPosition *v, struct EnemyParticleParams *params)
{
    s32 angle = params->angle;
    s32 magnitude = params->speed;
    v->x = Math_ShiftTowardZero(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff], 12);
    v->y = 0;
    v->z = Math_ShiftTowardZero(magnitude * gUnk_086149C4[angle & 0xff], 12);
}
s32 EnemyParticle_UpdatePool(struct EnemyParticlePool *pool)
{
    struct EnemyParticle *base, *cursor;
    s32 i;
    union {
        u32 value;
        u16 half[2];
    } animOffset = {0};
    animOffset.half[0] = (u32) & ((struct EnemyParticle *)0)->animation;
    if (Engine_PauseBits(0x400))
        return 0;
    {
        u32 busy = 4;
        if (gPlayerSlotStatus[0] & busy)
            return 0;
    }
    base = pool->slots;
    for (i = 0, cursor = base; i <= 15; i++, cursor++, base++) {
        u32 bit = 1 << i;
        u32 live = pool->mask & bit;
        s16 *ground = &base->params.groundHeight;
        if (live) {
            struct ContactSwarmSprite *o = &base->sprite;
            struct EnemyParticleParams *params = &base->params;
            struct ContactPosition *v;
            struct ContactPosition *pos;
            s32 h;
            struct ContactSwarmSprite *sprite;
            struct ContactPosition *g;
            u32 recompute;
            s32 result;
            result = sub_08249240(base, (void *)i, cursor->params.callback);
            sprite = o;
            if (result == 1)
                goto release;
            recompute = cursor->flags & 1;
            v = &base->velocity;
            if (recompute) {
                struct ContactPosition *w = v;
                ComputeVelocity(w, params);
            }
            pos = &o->position;
            o->position.x += cursor->velocity.x;
            pos->y += v->y;
            pos->z += v->z;
            params->verticalSpeed -= params->gravity;
            h = params->height += params->verticalSpeed;
            g = pos;
            if (h < 0)
                o->position.y = *ground - ((~h + 1) >> 12);
            else
                o->position.y = (h >> 12) + *ground;
            if (o->position.y < 0)
                goto release;
            {
                s32 x = o->position.x >> 8, z = g->z >> 8, index;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    index = 0;
                else
                    index = gTerrainMap->rows[z] + x;
                if ((u32)((*cell(index) & 15) << 8) > o->position.y) {
                release:
                    EnemyParticle_ReleaseSlot(pool, base, i);
                    continue;
                }
            }
            if (cursor->params.script) {

                struct DisplaySequenceSprite *s = (void *)sprite;
                struct DisplaySequenceState *state = (void *)((u8 *)base + animOffset.value);
                const u16 *command;
                command = cursor->animation.commands + cursor->animation.index;
                s->frame = *command >> 6;
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = cursor->animation.flags;
                    bit = 1;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        cursor->sprite.flags |= 4;
                    else
                        cursor->sprite.flags &= ~4;
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = state->flags;
                    bit = 2;
                    first = (u8)(control.raw & bit);
                    if (first != (((*command & 0x30) >> 4) & bit))
                        s->flags |= 8;
                    else
                        s->flags &= ~8;
                }
                if (++state->elapsed >= state->duration) {
                    state->elapsed = 0;
                    if (state->flags & 4) {
                        if (state->index == 0) {
                            if (state->flags & 8)
                                state->elapsed = state->duration;
                            else
                                state->index = state->frameCount - 1;

                        } else {
                            state->index--;
                        }
                    } else if (++state->index >= state->frameCount) {
                        if ((u8)(state->flags & 8)) {
                            state->elapsed = state->duration;
                            state->index = state->frameCount - 1;

                        } else {
                            state->index = 0;
                        }
                    }
                    command = state->commands + state->index;
                    DisplaySequence_UpdateWeight(state, command);
                    if (!state->duration)
                        state->duration = 1;
                }
            }
            cursor->timer++;
        }
    }
    return 0;
}
