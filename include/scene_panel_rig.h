#ifndef GUARD_SCENE_PANEL_RIG_H
#define GUARD_SCENE_PANEL_RIG_H
#include "scene_panel.h"
#include "contact_actor.h"
#include "display_sequence_inline.h"

struct SceneRigParentView {
    u8 unknown0[0x1E];
    u8 subjectCount, activeCount;
    u8 unknown20[0xBC8];
    struct ScenePanelSubject *subjects[4];
    u16 resourceScale;
    u8 unknownBFA[0x16];
    u8 mode;
    u8 unknownC11[0x731];
    u8 activated, warningStage;
    u16 warningSounds[4];
};

struct SceneRigContact {
    u8 unknown0[0x34];
    u32 typeFlags, reactionFlags;
    u16 power, unknown3E;
    u8 unknown40[2], direction;
};
u32 ScenePanelRig_IsSubjectState(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_SetSubjectState(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_RequestSubject(struct ScenePanelRig *, u32, u32);
void ScenePanelRig_SetState(struct ScenePanelRig *, u32);
void ScenePanelRig_RandomizeMovement(struct ScenePanelRig *);
s32 ScenePanelRig_SelectActiveSubject(struct ScenePanelRig *);
u32 ScenePanelRig_GetParentMode(struct ScenePanelRig *);
void ScenePanelRig_HideSprites(struct ScenePanelRig *);
void ScenePanelRig_ShowSprites(struct ScenePanelRig *);
void ScenePanelRig_InitMovement(struct ScenePanelRig *);
u32 ScenePanelRig_GetActiveCount(struct ScenePanelRig *);
void ScenePanelSubject_SetState(struct ScenePanelSubject *, u32);
extern const s16 gUnk_086149C4[];
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

/* The rig sprites' sequence step: as PlayerCue_StepSequence, but the zero-delay test masks the
 * computed delay with 255 (a constant the sprite loops hoist) instead of testing the stored byte. */
static inline void ScenePanelRig_StepSprite(struct SceneRigSprite *sprite)
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
