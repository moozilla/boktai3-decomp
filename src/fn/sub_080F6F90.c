#include "contact_swarm.h"
#include "display_sequence_inline.h"

struct SwordSize {
    u32 x : 16, y : 16, z : 16;
};
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
static inline s32 ReadArgument(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}
struct SwordComponent {
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider attack;
    struct ContactCollider body;
    u16 fieldF0;
    u16 fieldF2;
    u16 fieldF4;
    u8 padF6[0x2];
    u16 fieldF8;
    u16 fieldFA;
    u16 fieldFC;
    u8 padFE[0xC];
    u16 field10A;
    u8 pad10C[0xC];
    u8 field118;
    u8 field119;
    u8 field11A;
    u8 pad11B[0x3];
    u8 field11E;
    u8 field11F;
    u8 pad120[0x6];
    u16 field126;
    u16 field128;
    u8 field12A;
    u8 field12B;
    u8 pad12C[0x34];
    u32 field160;
    u8 pad164[0x4];
    u32 field168;
    u8 pad16C[0x14];
    u32 field180;
    u32 field184;
    u8 pad188[0xC];
};
// confidence=low: the four children are the active sword components updated at 080F63C4.
void Ivy_OnChildDestroy(void);
void sub_080F4708(void);
static inline void Sword_SetFrame(struct DisplaySequenceSprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
static inline void Sword_SetFrameIndex(struct DisplaySequenceState *state, u16 index)
{
    state->index = index;
}
static inline u16 Sword_ReadCommand(const u16 *command)
{
    return *command;
}
static inline u8 Sword_GetFrameCount(struct DisplaySequenceState *state)
{
    return state->frameCount;
}
/* The four child sprites advance one shared playback state during setup. */
static inline u32 Sword_StepSequence(struct DisplaySequenceSprite *owner,
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
    Sword_SetFrame(owner, Sword_ReadCommand(command) >> 6);
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
                    Sword_SetFrameIndex(state, Sword_GetFrameCount(state) - 1);
                ended = 1;
            } else {
                Sword_SetFrameIndex(state, state->index - 1);
                ended = 0;
            }
        } else {
            if (++state->index >= Sword_GetFrameCount(state)) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    Sword_SetFrameIndex(state, Sword_GetFrameCount(state) - 1);
                } else {
                    Sword_SetFrameIndex(state, 0);
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(Sword_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

struct SwordAttackSetup {
    u8 unknown0[0x4BC];
    struct SwordComponent components[4];
    u8 unknownB0C[0xD14 - 0xB0C];
    s16 fieldD14, bodyD16, fieldD18, fieldD1A, bodyD1C, bodyD1E;
    s16 attackD20, attackD22, attackD24, attackD26;
    u8 unknownD28[4];
    u8 count;
    u8 unknownD2D[0xD55 - 0xD2D];
    u8 variant;
    u16 paletteBank;
    u8 unknownD58[0xD7E - 0xD58];
    u8 fieldD7E;
};

static inline void Sword_ClearByte(u8 *field)
{
    *field = 0;
}

void Sword_InitAttackComponents(struct SwordAttackSetup *p)
{
    struct SwordSize size;
    struct SwordSize offset;
    struct DisplaySequenceState sequence;
    s32 i;
    u32 zero;
    size.x = 100;
    size.y = 100;
    size.z = 100;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (Script_SeekToKeyword('c')) {
        p->fieldD14 = ReadArgument();
        p->bodyD16 = ReadArgument();
        p->fieldD18 = ReadArgument();
        p->fieldD1A = ReadArgument();
        p->fieldD7E = ReadArgument();
        p->bodyD1C = ReadArgument();
        p->bodyD1E = ReadArgument();
    }
    if (Script_SeekToKeyword('C')) {
        p->attackD20 = ReadArgument();
        p->attackD22 = ReadArgument();
        p->attackD24 = ReadArgument();
        p->attackD26 = ReadArgument();
    }
    if (Script_SeekToKeyword('n'))
        p->count = Script_GetValue();
    else
        p->count = 4;
    Anim_SetSequence(&sequence, (void *)ScriptProc_Start(0x922E, 0x3F72), 2,
                     (u8)((p->variant) << 2), 0);
    for (i = 0, zero = i; i < 4; i++) {
        struct SwordComponent *child = &p->components[i];
        struct DisplaySequenceSprite *sprite = &child->sprite;
        void *graphics = &child->graphics;
        void *collider;
        child->fieldF0 = zero;
        child->fieldF2 = zero;
        child->fieldF4 = zero;
        child->fieldF8 = zero;
        child->fieldFA = 1;
        child->fieldFC = zero;
        child->field119 = 0;
        child->field10A = zero;
        child->field11A = 0;
        Sword_ClearByte(&child->field12A);
        Sword_ClearByte(&child->field12B);
        child->field126 = zero;
        child->field128 = zero;
        child->field168 = zero;
        AnimData_Load(graphics, 0x5291);
        Sprite_Init(child, graphics, 0);
        Sword_StepSequence(sprite, &sequence);
        sprite->flags = 3;
        AnimData_SelectPaletteBank((void *)sprite->resource, p->paletteBank);
        child->field118 = i;
        child->field11E = zero;
        child->field11F = zero;
        child->field119 |= 1;
        child->field160 = (u32)p;
        child->field180 = 20;
        child->field184 = zero;
        collider = &child->attack;
        Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(collider, Ivy_OnChildDestroy, child);
        Collider_Refresh(collider);
        Collider_SetAttackProperties(collider, p->attackD20, p->attackD22, 0x2000, p->attackD24,
                                     p->attackD26);
        child->attack.flags |= 4;
        collider = &child->body;
        Collider_Init(collider, 0, 0x5009, 0, 16, &size, &offset);
        Collider_SetBodyProperties(collider, p->bodyD16, p->bodyD1C, p->bodyD1E);
        Collider_SetCallback(collider, sub_080F4708, child);
        Collider_Refresh(collider);
        child->body.flags |= 4;
    }
}
