#include "scene_arc.h"
#include "display_sequence.h"
s32 __divsi3(s32, s32);
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
s32 Terrain_GetHeightAtPosition(struct ScenePoolVec32 *);
void Collider_SetFixedPositionWithYOffset(void *, struct ScenePoolVec32 *, u32);
void SpatialIndex_InsertEntry(void *);
void GroundEffect_DisableProjection(struct SceneGroundEffect *);
void SceneArc_ReleaseSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);
void SpriteEffect_CreateAtPosition(struct ScenePoolVec32 *, void *, void *, u32, u32, u32, u32, u32,
                                   u32, u32, u32, u32, u32, u32, u32);
struct ArcDirection {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 SignedShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}
static inline void ProjectArc(struct SceneArcSlot *slot, struct SceneArcSprite *sprite, s32 lift)
{
    s32 height = lift << 12;
    struct ScenePoolVec32 *position = &slot->position;
    struct SceneVec *screen = &sprite->screen;
    screen->x = (slot->position.x >> 12) - gSceneCamera->x;
    screen->y = ((position->z + (position->y - height)) >> 12) + 96;
    screen->z = position->z >> 12;
    sprite->screen.y = (u16)sprite->screen.y - lift;
}
static inline u8 PlayerSuspendsArc(void)
{
    if (gParticipantActors[0]) {
        u32 flags = gParticipantActors[0]->flags;
        if (flags & 4)
            return 1;
    }
    return 0;
}
static inline void MakeVelocity(struct SceneVec *out, s32 angle, s32 speed)
{
    out->x = SignedShift(gUnk_086149C4[(angle + 64) & 255] << 13, 12);
    out->y = SignedShift(gUnk_086149C4[angle & 255] * speed, 12);
    out->z = 0;
}
static inline u16 DisplaySequence_GetIndex(struct SceneArcAnimation *state)
{
    return state->frame;
}

static inline u32 DisplaySequence_ReadCommand(const u16 *command)
{
    return *command;
}

static inline u32 DisplaySequence_MaskCommand(u32 command, u32 mask)
{
    mask &= command;
    return mask;
}

/* Apply the current frame and flips, then advance the stored playback state.
 * The duration calculation uses the newly selected command's low four bits. */
static inline void StepArc(struct SceneArcSprite *owner, struct SceneArcAnimation *state)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->data;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->timer >= state->rate) {
        state->timer = 0;
        if (state->flags & 4) {
            if (state->frame == 0) {
                if (state->flags & 8)
                    state->timer = state->rate;
                else
                    state->frame = state->count - 1;
            } else {
                state->frame--;
            }
        } else {
            if (++state->frame >= state->count) {
                if (state->flags & 8) {
                    state->timer = state->rate;
                    state->frame = state->count - 1;
                } else {
                    state->frame = 0;
                }
            }
        }
        command = state->data + state->frame;
        state->factor = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->rate = (state->factor * (s32)state->scale) >> 6;
        if (state->rate == 0)
            state->rate = 1;
    }
}

static inline void HideSprite(struct SceneArcSprite *sprite)
{
    sprite->flags |= 1;
}
void SceneArc_UpdateSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, u32 index)
{
    void *collider = slot->collider;
    u32 frame = slot->frame;
    u32 duration = slot->flightDuration;
    struct SceneArcSprite *sprite;
    struct ScenePoolVec32 *position;
    u32 paused;
    if (frame <= duration) {
        u32 remainder;
        s32 lift;
        s32 terrain;
        s32 screenY;
        sprite = &slot->sprite;
        remainder = duration - frame;
        slot->position.x =
            __divsi3(((slot->target.x >> 12) * frame) + ((slot->start.x >> 12) * remainder),
                     duration)
            << 12;
        slot->position.y =
            __divsi3(((slot->target.y >> 12) * frame) + ((slot->start.y >> 12) * remainder),
                     slot->flightDuration)
            << 12;
        slot->position.z =
            __divsi3(((slot->target.z >> 12) * frame) + ((slot->start.z >> 12) * remainder),
                     slot->flightDuration)
            << 12;
        lift = SignedShift(
            gUnk_086149C4[Div(slot->frame << 7, slot->flightDuration) & 255] * slot->height, 12);
        ProjectArc(slot, sprite, lift);
        terrain = Terrain_GetHeightAtFixedPosition(&slot->position);
        screenY = sprite->screen.y;
        position = &slot->position;
        if (terrain < screenY) {
            sprite->screen.y = terrain;
            slot->grounded = 1;
            slot->frame = slot->endFrame;
            goto flightEnded;
        }
        if (slot->frame == slot->flightDuration) {
        flightEnded:
            Sound_PlaySongChecked(0x166);
        }
    } else {
        s32 lift;
        u32 remaining;
        u32 interval;
        struct ArcDirection direction;
        struct SceneVec velocity;
        s32 angle;
        s32 terrain;
        s32 screenY;
        sprite = &slot->sprite;
        lift = 0;
        remaining = duration - frame;
        interval = duration - slot->endFrame;
        direction.x = (slot->target.x >> 12) - (slot->start.x >> 12);
        direction.y = slot->target.y - (slot->start.y >> 12);
        direction.z = (slot->target.z >> 12) - (slot->start.z >> 12);
        angle = Math_Atan2(direction.x, direction.z);
        MakeVelocity(&velocity, angle, 8192);
        if (slot->grounded != 1) {
            slot->position.x += velocity.x;
            slot->position.y += velocity.y;
            slot->position.z += velocity.z;
        }
        if (slot->position.z <= 0x5fff)
            slot->position.z = 0x6000;
        else if (slot->position.z > 0x34000)
            slot->position.z = 0x34000;
        if (interval != 0) {
            s32 phase = Div(remaining << 7, interval);
            const s16 *table = gUnk_086149C4;
            lift = SignedShift(table[phase & 255], 10);
        }
        ProjectArc(slot, sprite, lift);
        terrain = Terrain_GetHeightAtFixedPosition(&slot->position);
        screenY = sprite->screen.y;
        position = &slot->position;
        if (terrain < screenY) {
            sprite->screen.y = terrain;
            slot->grounded = 1;
        }
    }
    paused = PlayerSuspendsArc();
    if (!paused) {
        if (slot->frame >= slot->endFrame) {
            Collider_SetFixedPositionWithYOffset(collider, position, 0);
            SpatialIndex_InsertEntry(collider);
            if (slot->frame == slot->endFrame) {
                Sound_PlaySongChecked(0x195);
                slot->position.y = -(Terrain_GetHeightAtPosition(position) << 12);
                SpriteEffect_CreateAtPosition(position, 0, 0, 0x8639, 0x75f6, paused, paused,
                                              paused, paused, 0x153, 30, paused, paused, 1, paused);
                HideSprite(&slot->sprite);
                GroundEffect_DisableProjection(&slot->ground);
            }
        }
        if (slot->frame == slot->releaseFrame) {
            SceneArc_ReleaseSlot(manager, slot, index);
            return;
        }
    }
    StepArc(&slot->sprite, &slot->animation);
    slot->frame++;
}
