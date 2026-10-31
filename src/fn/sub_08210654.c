#include "global.h"
#include "pink_spine_playback.h"
#include "display_sequence_inline.h"
// confidence=low: prefix follows the neighboring creator; species identity remains tentative.
void ProjectileEmitter_WaitForPlayerApproach(void);
void ProjectileEmitter_EmitAttackFrame(void);
s32 PinkSpine_InitCollider(struct PinkSpineState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
static inline void PinkSpine_GetSpriteFlags(u32 **out, struct ScreamerSprite *sprite)
{
    *out = &sprite->flags;
}
static inline u32 PinkSpine_StepSequence(u32 *flipFlags, struct PinkSpineState *actor,
                                         struct ScreamerSprite *owner,
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
            actor->sprite.flags |= 4;
        else
            actor->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            *flipFlags |= 8;
        else
            *flipFlags &= ~8;
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

s32 PinkSpine_InitFromScript(struct PinkSpineState *p, void *resource)
{
    void *graphics;
    void *sprite;
    void *initialSprite;
    void *sequences;
    s32 initialDead;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        p->nearRadiusSquared = Script_GetValue();
        p->farRadiusSquared = Script_GetValue();
        p->nearRadiusSquared *= p->nearRadiusSquared;
        p->farRadiusSquared *= p->farRadiusSquared;
    } else {
        p->nearRadiusSquared = 0x90000;
        p->farRadiusSquared = 0x10000;
    }
    if (Script_SeekToKeyword('l')) {
        p->f136 = Script_GetValue();
        if (p->f136 < 0)
            p->f13A = 1;
    }
    if (Script_SeekToKeyword('a')) {
        p->f124 = Script_GetValue();
        p->f126 = Script_GetValue();
        p->f128 = Script_GetValue();
        p->f12E = Script_GetValue();
    } else {
        p->f124 = 30;
        p->f126 = 0;
        p->f128 = 30;
        p->f12E = 24;
    }
    if (Script_SeekToKeyword('S'))
        p->f138 = Script_GetValue();
    if (Script_SeekToKeyword('D'))
        p->f158 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->f14C = Script_GetValue();
        p->f150 = Script_GetValue();
        p->f154 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->f13C = Script_GetValue();
        p->f13E = Script_GetValue();
    }
    p->f140 = resource;
    graphics = p->graphics;
    AnimData_Load(graphics, 0x7E7D);
    initialSprite = &p->sprite;
    Sprite_Init(initialSprite, graphics, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(graphics, 0x1BD);
    sequences = ScriptProc_Start(0x922E, 0x5184);
    p->f15C = sequences;
    initialDead = p->f13A;
    sprite = initialSprite;

    if (!initialDead) {
        p->update = ProjectileEmitter_WaitForPlayerApproach;
        Anim_SetSequence(&p->sequence, sequences, 0xB, 0, 0);
    } else {
        p->update = ProjectileEmitter_EmitAttackFrame;
        p->f134 = 1;
        Anim_SetSequence(&p->sequence, p->f15C, 0xD, 0, 0);
    }
    {

        u32 *flipFlags;
        PinkSpine_GetSpriteFlags(&flipFlags, sprite);
        PinkSpine_StepSequence(flipFlags, p, sprite, &p->sequence);
    }
    if (!p->f13A)
        PinkSpine_InitCollider(p);
    if (p->f138) {
        SceneBody_Init(p->body, p->f140, &p->sprite.position, 0, 4, p);
        SceneBody_SetSprite(p->body, sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    p->f136 = 1;
    /* This initializer has no explicit return-value assignment in the ROM. */
}
