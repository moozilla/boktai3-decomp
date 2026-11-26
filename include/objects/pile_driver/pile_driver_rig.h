#ifndef GUARD_PILE_DRIVER_RIG_H
#define GUARD_PILE_DRIVER_RIG_H
#include "objects/pile_driver/pile_driver.h"
#include "scene/collision/contact_actor.h"
#include "engine/video/display_sequence_inline.h"

struct SceneRigParentView {
    u8 unknown0[0x1E];
    u8 subjectCount, activeCount;
    u8 unknown20[0xBC8];
    struct PileDriverSubject *subjects[4];
    u16 resourceScale;
    u8 unknownBFA[0x16];
    u8 mode;
    u8 unknownC11[0x731];
    u8 activated, warningStage;
    u16 warningSounds[4];
};

u32 PileDriverRig_IsSubjectState(struct PileDriverRig *, u32, u32);
void PileDriverRig_SetSubjectState(struct PileDriverRig *, u32, u32);
void PileDriverRig_RequestSubject(struct PileDriverRig *, u32, u32);
void PileDriverRig_SetState(struct PileDriverRig *, u32);
void PileDriverRig_RandomizeMovement(struct PileDriverRig *);
s32 PileDriverRig_SelectActiveSubject(struct PileDriverRig *);
u32 PileDriverRig_GetParentMode(struct PileDriverRig *);
void PileDriverRig_HideSprites(struct PileDriverRig *);
void PileDriverRig_ShowSprites(struct PileDriverRig *);
void PileDriverRig_InitMovement(struct PileDriverRig *);
u32 PileDriverRig_GetActiveCount(struct PileDriverRig *);
void PileDriverSubject_SetState(struct PileDriverSubject *, u32);
extern const s16 gUnk_086149C4[];
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

/* The rig sprites' sequence step: as PlayerCue_StepSequence, but the zero-delay test masks the
 * computed delay with 255 (a constant the sprite loops hoist) instead of testing the stored byte. */
static inline void PileDriverRig_StepSprite(struct SceneRigSprite *sprite)
{
    struct DisplaySequenceState *state = &sprite->sequence;
    const u16 *command;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 delay = (state->frameWeight * (s32)state->speed) >> 6;
            state->duration = delay;
            if (!(delay & 255))
                state->duration = 1;
        }
    }
}

#endif
