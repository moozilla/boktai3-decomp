#include "global.h"

#include "display_animation.h"
#include "display_sprite_playback.h"
#include "terrain_cell_probe.h"

/* Tentative: animate a pooled effect, integrate its velocity and stop it at terrain. */
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
    ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite, state);
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
