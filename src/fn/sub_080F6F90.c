// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: initializes the four active sword attack components.

// Animation mask lifetime, collider argument addresses and register allocation still differ.
#define H(p, off) (*(u16 *)((u8 *)(p) + (off)))
#define S(p, off) (*(s16 *)((u8 *)(p) + (off)))
#define B(p, off) (*((u8 *)(p) + (off)))
#define W(p, off) (*(u32 *)((u8 *)(p) + (off)))
#define P(p, off) ((u8 *)(p) + (off))
struct SwordSize {
    u32 x : 16, y : 16, z : 16;
};
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
s32 Collider_Refresh(void *);
static inline s32 ReadArgument(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}
static inline void Step(struct DisplaySequenceSprite *sprite, struct DisplaySequenceState *state,
                        const u16 *commands)
{
    const volatile u16 *command;
    u32 duration, flags;
    command = (void *)((state->index << 1) + (u32)state->commands);
    sprite->frame = *command >> 6;
    {
        u32 first = state->flags & 1;
        u32 bit = 1;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 bit = 2;
        u32 first;
        flags = state->flags;
        first = (u8)(flags & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 temporary = 0xFFFF;
        u32 mask;
        state->elapsed = elapsed;
        mask = temporary;
        duration = state->duration;
        if ((u16)elapsed >= duration) {
            state->elapsed = 0;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    if (flags & 8)
                        state->elapsed = duration;
                    else
                        state->index = state->frameCount - 1;
                } else
                    state->index = index - 1;
            } else {
                u32 index = state->index + 1;
                state->index = index;
                if ((index & mask) >= state->frameCount) {
                    if ((u8)(flags & 8)) {
                        state->elapsed = duration;
                        state->index = state->frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

struct SwordCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4C];
};
struct SwordComponent {
    struct DisplaySequenceSprite sprite;
    u8 graphics[0x1C];
    struct SwordCollider attack;
    struct SwordCollider body;
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
static inline void Step_InitAttackComponents(struct DisplaySequenceSprite *sprite,
                                             struct DisplaySequenceState *state,
                                             const u16 *commands)
{
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    command = (void *)((state->index << 1) + (u32)state->commands);
    sprite->frame = *command >> 6;
    {
        u32 first = state->flags & 1;
        u32 bit = 1;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 bit = 2;
        u32 first;
        flags = state->flags;
        first = (u8)(flags & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 temporary = 0xFFFF;
        u32 mask;
        state->elapsed = elapsed;
        mask = temporary;
        duration = state->duration;
        if ((u16)elapsed >= duration) {
            state->elapsed = 0;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    if (flags & 8)
                        state->elapsed = duration;
                    else
                        state->index = state->frameCount - 1;
                } else
                    state->index = index - 1;
            } else {
                u32 index = state->index + 1;
                state->index = index;
                if ((index & mask) >= state->frameCount) {
                    if ((u8)(flags & 8)) {
                        state->elapsed = duration;
                        state->index = state->frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

void Sword_InitAttackComponents(u8 *input)
{
    u8 *p = input;
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
        *((u16 *)(((u8 *)p) + 0xD14)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD16)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD18)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD1A)) = ReadArgument();
        *(((u8 *)p) + 0xD7E) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD1C)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD1E)) = ReadArgument();
    }
    if (Script_SeekToKeyword('C')) {
        *((u16 *)(((u8 *)p) + 0xD20)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD22)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD24)) = ReadArgument();
        *((u16 *)(((u8 *)p) + 0xD26)) = ReadArgument();
    }
    if (Script_SeekToKeyword('n'))
        *(((u8 *)p) + 0xD2C) = Script_GetValue();
    else
        *(((u8 *)p) + 0xD2C) = 4;
    Anim_SetSequence(&sequence, (void *)ScriptProc_Start(0x922E, 0x3F72), 2,
                     (u8)((*(((u8 *)p) + 0xD55)) << 2), 0);
    for (i = 0; i < 4; i++) {
        struct SwordComponent *child = (void *)(((u8 *)p) + (0x4BC + (i * 0x194)));
        struct DisplaySequenceSprite *sprite = &child->sprite;
        void *graphics = child->graphics;
        void *collider;
        child->fieldF0 = zero;
        child->fieldF2 = zero;
        child->fieldF4 = zero;
        child->fieldF8 = zero;
        child->fieldFA = 1;
        child->fieldFC = zero;
        child->field119 = 0;
        child->field10A = zero;
        do {
            child->field11A = 0;
        } while (0);
        child->field12A = 0;
        child->field12B = 0;
        child->field126 = zero;
        child->field128 = zero;
        child->field168 = zero;
        AnimData_Load(graphics, 0x5291);
        Sprite_Init(child, graphics, 0);
        Step_InitAttackComponents(sprite, &sequence, sequence.commands);
        sprite->flags = 3;
        AnimData_SelectPaletteBank((void *)sprite->resource, *((u16 *)(((u8 *)p) + 0xD56)));
        child->field118 = i;
        child->field11E = 0;
        child->field11F = 0;
        child->field119 |= 1;
        child->field160 = (u32)p;
        child->field180 = 20;
        child->field184 = zero;
        collider = &child->attack;
        Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(collider, Ivy_OnChildDestroy, child);
        Collider_Refresh(collider);
        Collider_SetAttackProperties(collider, *((s16 *)(((u8 *)p) + 0xD20)),
                                     *((s16 *)(((u8 *)p) + 0xD22)), 0x2000,
                                     *((s16 *)(((u8 *)p) + 0xD24)), *((s16 *)(((u8 *)p) + 0xD26)));
        child->attack.flags |= 4;
        collider = &child->body;
        Collider_Init(collider, 0, 0x5009, 0, 16, &size, &offset);
        Collider_SetBodyProperties(collider, *((s16 *)(((u8 *)p) + 0xD16)),
                                   *((s16 *)(((u8 *)p) + 0xD1C)), *((s16 *)(((u8 *)p) + 0xD1E)));
        Collider_SetCallback(collider, sub_080F4708, child);
        Collider_Refresh(collider);
        child->body.flags |= 4;
    }

    zero = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Sword_InitAttackComponents);
#endif
