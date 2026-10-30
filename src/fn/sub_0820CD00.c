#include "global.h"
#include "screamer_playback.h"
#include "display_sequence_inline.h"
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
void SceneDrawablePair_SetFlags(void);
s32 Screamer_InitColliders(struct ScreamerState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline u32 Screamer_StepSequence(struct ScreamerSprite *owner,
                                        struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
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
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

s32 Screamer_InitFromScript(struct ScreamerState *p, void *resource)
{
    struct ActorPosition position;
    void *graphics;
    void *sequences;
    s32 value;
    s16 side;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->f1C4 = Script_GetValue();
        p->f1C6 = Script_GetValue();
        p->f1C8 = Script_GetValue();
    } else {
        p->f1C4 = 30;
        p->f1C6 = 10;
        p->f1C8 = 30;
    }
    if (Script_SeekToKeyword('l')) {
        value = Script_GetValue();
        p->f1E6 = value;
        p->f1E4 = value;
        if (((s16)value) < 0)
            p->f1F6 = 1;
    } else {
        p->f1E6 = 120;
        p->f1E4 = 120;
    }
    if (Script_SeekToKeyword('i'))
        p->f1CC = Script_GetValue();
    else
        p->f1CC = 120;
    if (Script_SeekToKeyword('r'))
        p->f1DA = Script_GetValue();
    else
        p->f1DA = 256;
    if (Script_SeekToKeyword('w')) {
        p->f1D4 = Script_GetValue();
        p->f1D0 = Script_GetValue();
    } else {
        p->f1D4 = 30;
        p->f1D0 = 3;
    }
    if (Script_SeekToKeyword('h')) {
        p->f1D8 = Script_GetValue();
        p->f1D8 = (side = (p->f1D8 - 1) / 2);
        if ((u32)(u16)side <= 3) {
            if ((u32)(u16)side <= 1)
                p->f1D8 = 0;
            else
                p->f1D8 = 1;
        } else
            p->f1D8 = 1;

    } else
        p->f1D8 = 0;
    if (Script_SeekToKeyword('S'))
        p->f1EE = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->f1E8 = Script_GetValue();
        p->f1EA = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->f1EC = Script_GetValue();
    else
        p->f1EC = 2;
    if (Script_SeekToKeyword('D'))
        p->f214 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->f208 = Script_GetValue();
        p->f20C = Script_GetValue();
        p->f210 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->f1F0 = Script_GetValue();
        p->f1F2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->f1F4 = Script_GetValue();
        if (p->f1F4 > 99)
            p->f1F4 = 99;
        if (p->f1F4 <= 0)
            p->f1F4 = 1;
    } else
        p->f1F4 = 0;
    p->f1F8 = resource;
    graphics = p->graphics;
    AnimData_Load(graphics, 0x7E7D);
    Sprite_Init(&p->sprite, graphics, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(graphics, 0x1C4);
    *(((u8 *)(&p->sprite)) + 7) = 3;
    sequences = ScriptProc_Start(0x922E, 0x5184);
    p->f218 = sequences;
    if (!p->f1F6) {
        p->update = Screamer_WaitForPlayerApproach;
        Anim_SetSequence(&p->sequence, sequences, 0x1A, (u8)p->f1D8, 8);
    } else {
        p->update = SceneDrawablePair_SetFlags;
        p->f1E0 = 1;
        Anim_SetSequence(&p->sequence, p->f218, 0x1A, (u8)p->f1D8, 8);
    }
    Screamer_StepSequence(&p->sprite, &p->sequence);
    if (!p->f1F6)
        Screamer_InitColliders(p);
    if (!p->f1D8)
        p->sprite.position.x -= 16;
    else
        p->sprite.position.z -= 16;
    if (p->f1EE) {
        struct ActorPosition *offsetPosition;
        offsetPosition = &position;
        *offsetPosition = p->sprite.position;
        if (!p->f1D8)
            offsetPosition->x += 128;
        else
            offsetPosition->z += 128;
        SceneBody_Init(p->body, p->f1F8, &position, 0, 4, p);
        SceneBody_SetSprite(p->body, &p->sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    if (p->f1DA > 0)
        p->f1FC = p->f1DA * p->f1DA;
    else
        p->f1FC = p->f1DA;
}
