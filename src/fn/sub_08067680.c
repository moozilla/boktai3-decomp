#include "scene_actor_control.h"
#include "ram.h"
#include "display_sprite_playback.h"
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *);
void SceneActorControl_DecodeDirection(const void *, u8 *, u8 *);
s32 Region_FindContaining(const struct ContactPosition *);
void Sprite_InitUnlinked(void *, void *, u32);
u32 AreaEffect_HasEnabledTerrainValue(s32);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *, s32, s32);
static inline void Step(struct SceneActorControlSlot *slot, struct SceneActorControlSlot *sprite,
                        struct DisplaySequenceState *state)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    slot->tile = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            slot->spriteFlags |= 4;
        else
            slot->spriteFlags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->spriteFlags |= 8;
        else
            sprite->spriteFlags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else
                state->index--;
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else
                    state->index = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}
s32 SceneActorControl_CreateFromPosition(const struct SceneActorControlInput *input)
{
    struct SceneActorControlManager *found = gSceneActorControlManager;
    struct SceneActorControlManager *p;
    struct SceneActorControlSlot *slot, *sprite;
    u8 unused, sequence;
    if (!found) {
        p = SceneActorControl_EnsureSingleton();
        if (!p)
            return -1;
    } else
        p = found;
    slot = SceneActorControl_AllocateSlot(p);
    if (!slot)
        return -1;
    slot->kind = 1;
    SceneActorControl_DecodeDirection(input, &unused, &sequence);
    slot->savedPosition = input->position;
    slot->scene = Region_FindContaining(&slot->savedPosition);
    slot->flags = 0;
    slot->parameter = 0;
    sprite = slot;
    slot->position = slot->savedPosition;
    Sprite_InitUnlinked(slot, p->graphics, 0);
    Anim_SetSequence(&slot->sequence, p->sequenceResource, 1, 0, sequence);
    Step(slot, sprite, &slot->sequence);
    sprite->spriteFlags &= ~1;
    sprite->priority = 1;
    if (AreaEffect_HasEnabledTerrainValue(slot->scene))
        slot->flags |= 0x20;
    SceneActorControl_InitCollider(slot, 0, 0);
    return slot->index;
}
