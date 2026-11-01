#include "global.h"

// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#include "terrain_cell_probe.h"

/* Tentative: animate a pooled effect, integrate its velocity and stop it at terrain. */
// MATCHING_CONCESSION: register-pinning
struct EffectPosition {
    s16 x, y, z, pad;
};
struct SceneColliderEffect {
    u32 flags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
    struct EffectPosition position;
    u8 unk_24[8];
    struct DisplaySequenceState sequence;
    struct EffectPosition velocity;
    u8 collider[0x54];
    s16 motionFlags;
    u8 active;
    s8 slot;
    u8 unk_9C[4];
    s16 timer;
};
struct SceneColliderEffectPool {
    u8 unk_00[0x38];
    u32 used;
};
static inline u8 TestFlag(struct SceneColliderEffect *effect, s32 mask)
{
    if (effect->motionFlags & mask)
        return 1;
    return 0;
}
static inline u32 GroundHeight(struct EffectPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8, index;
    u8 *cell;
    u32 type;
    s32 height;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = cellProps(index);
    type = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (type) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2:
            height -= *(u8 *)&position->x;
            break;
    }
    return height;
}
static inline void AddPosition(struct EffectPosition *out, struct EffectPosition *a,
                               struct EffectPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

// confidence=low: animates a pooled collider effect, integrates its velocity and stops it at terrain.
void sub_08121504(struct SceneColliderEffectPool *pool, struct SceneColliderEffect *effect)
{
    struct SceneColliderEffect *sprite = effect;
    struct DisplaySequenceState *state;
    u32 ended;
    struct EffectPosition *position;
    void *collider;
    sprite->flags &= ~1;
    state = &effect->sequence;
    if (!TestFlag(effect, 0x20)) {
        state->index = 0;
        state->elapsed = 0;
    }
    {

        const volatile u16 *command; /* preserve observed packed-command reads */
        u32 duration, flags, bit;
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
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
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
                sprite->flags |= 8;
            else
                sprite->flags &= ~8;
        }
        {
            u32 elapsed = state->elapsed + 1;
            u32 mask;
            register u32 temp asm("r3");
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
                        ended = 1;
                    } else {
                        state->index = index - 1;
                        ended = 0;
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
                        ended = 1;
                    } else {
                        ended = 0;
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
                ended = 0;
            }
        }
    }
    if (ended) {
        effect->timer = 0;
        sprite->flags |= 1;
        pool->used &= ~(1 << effect->slot);
        effect->active = 0;
        Sprite_Remove(sprite);
        effect->slot = -1;
    } else {
        u8 stopped = TestFlag(effect, 0x20);
        position = &sprite->position;
        if (!stopped) {
            struct EffectPosition next = sprite->position;
            u16 oldHeight, newHeight;
            AddPosition(&next, &next, &effect->velocity);
            effect->velocity.y--;
            oldHeight = GroundHeight(&sprite->position);
            newHeight = GroundHeight(&next);
            sprite->position = next;
            if (sprite->position.y < (s16)oldHeight || sprite->position.y < (s16)newHeight) {
                sprite->position.x -= effect->velocity.x;
                sprite->position.z -= effect->velocity.z;
                effect->motionFlags |= 0x20;
                effect->velocity.x = 0;
                effect->velocity.y = 0;
                effect->velocity.z = 0;
            }
        }
        collider = effect->collider;
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
    }
}
