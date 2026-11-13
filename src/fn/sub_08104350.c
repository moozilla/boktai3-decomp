#include "global.h"
#include "display_sprite_playback.h"
#include "ram.h"
struct Frames {
    u16 *data;
    u8 flags;
    u8 count;
    u8 multiplier;
    u8 duration;
    u16 index;
    u16 pada;
    u16 rate;
    u16 timer;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
    u8 pad12[10];
    s16 x;
    s16 y;
    s16 z;
    s16 pad22;
};
struct Context {
    u8 pad[0x18];
    void *resource;
};
struct Context *Pool184_CreateManager(void);
struct Member;
struct Member *Pool184_Allocate(struct Context *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
u32 Script_GetKeywordValue(u32, u32);
u32 Script_SeekToKeyword(u32);
void *Script_GetPc(void);
s32 Script_GetValue(void);
u8 Pool184_TestStatusFlags(void *, u32);
s32 ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Memory_Clear(void *, u32);
void sub_08105194(void *, s32, s32, s32);
struct Position {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};
struct Member {
    struct Render render;
    u8 unknown24[8];
    struct Frames frames;
    u8 graphics[0x1C];
    u8 unknown58[84];
    u8 shadow[0x40];
    u8 unknownEC[44];
    struct Position origin;
    u8 unknown120[24];
    s32 rangeSquared;
    u32 scriptP;
    u32 scriptN;
    s16 damageStep;
    u8 damageIndex;
    u8 damageCount;
    u8 unknown148[2];
    s16 health;
    s16 maxHealth;
    u8 unknown14E[2];
    u8 optionA;
    u8 debrisCount;
    u16 state;
    u8 unknown154[2];
    u16 scriptM;
    s8 shadowId;
    u8 unknown159[1];
    u8 animationFlags;
    u8 unknown15B[3];
    u8 started;
    u8 unknown15F[1];
    u32 scriptI;
    u32 arguments[8];
};
static inline s32 ReadOptional(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
void Anim_SetSequence(struct Frames *, void *, u16, int, int);
s32 Div(s32, s32);
static inline u32 ActorGroup_StepAnimation(struct Member *m, struct Render *owner,
                                           struct Frames *state)
{
    struct Frames *f = state;
    const u16 *command;
    u32 ended;
    command = m->frames.data + f->index;
    owner->tile = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = f->flags;
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
        control.raw = f->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++f->timer >= f->duration) {
        u32 commandWord;
        f->timer = 0;
        if (f->flags & 4) {
            if (f->index == 0) {
                if (f->flags & 8)
                    f->timer = f->duration;
                else
                    f->index = f->count - 1;
                ended = 1;
            } else {
                f->index--;
                ended = 0;
            }
            command = f->data + f->index;
            commandWord = *command;
        } else {
            if (++f->index >= f->count) {
                if ((u8)(f->flags & 8)) {
                    f->timer = f->duration;
                    f->index = f->count - 1;
                    ended = 1;
                } else {
                    f->index = 0;
                    ended = 1;
                }
            } else
                ended = 0;
            command = f->data + f->index;
            commandWord = *command;
        }
        f->multiplier = DisplaySequence_MaskCommand(commandWord, 15);
        f->duration = (f->multiplier * (s32)f->rate) >> 6;
        if (!f->duration)
            f->duration = 1;
    } else
        ended = 0;
    return ended;
}
// confidence=low: script options initialize a member of Pool184's actor group.
void ActorGroup_CreateFromScript(void)
{
    struct Context *pool = gActorGroupPool;
    struct Member *p;
    struct Render *render;
    s32 c;
    s32 a;
    s32 b;
    u32 zero;
    u32 one;
    struct Frames *sequence;
    s32 i;
    s32 value;
    if (!pool) {
        pool = Pool184_CreateManager();
        if (!pool)
            return;
    }
    p = Pool184_Allocate(pool);
    if (!p)
        return;
    render = &p->render;

    {
        void *graphics = (u8 *)&p->graphics;
        AnimData_Load(graphics, 0x9385);
        Sprite_Init(p, graphics, 0);
    }
    {
        u8 *started = &p->started;
        zero = 0;
        one = 1;
        *started = one;
    }
    p->damageIndex = zero;
    p->scriptM = Script_GetKeywordValue('m', 0);
    if (Pool184_TestStatusFlags(p, 2))
        p->state = 2;
    else
        p->state = one;
    if (Script_SeekToKeyword('p')) {
        render->x = Script_GetValue();
        render->y = Script_GetValue();
        render->z = Script_GetValue();
    }
    p->origin = *(struct Position *)&render->x;
    p->animationFlags = Script_GetKeywordValue('r', 0);
    p->scriptN = Script_GetKeywordValue('n', 0);
    value = Script_GetKeywordValue('a', 256);
    p->rangeSquared = value * value;
    p->damageCount = Script_GetKeywordValue('b', 0);
    if (Script_SeekToKeyword('A')) {
        p->health = ReadOptional();
        p->debrisCount = ReadOptional();
        p->optionA = ReadOptional();
        a = ReadOptional();
        b = ReadOptional();
        c = ReadOptional();
    } else {
        p->health = 100;
        p->debrisCount = 3;
        p->optionA = 0;
        a = 0;
        b = 0;
        c = 0;
    }
    p->maxHealth = p->health;
    p->damageStep = Div(p->health, p->damageCount - 1);
    if (Script_SeekToKeyword('P')) {
        u32 *dest = (u32 *)((u8 *)&p->scriptP);
        *dest = ReadOptional();
    }
    {
        u32 flag = Pool184_TestStatusFlags(p, 2);
        if (flag)
            p->shadowId = ScaledShadow_Initialize((u8 *)&p->shadow, &p->render.x, 64, 16, 96, 100,
                                                  2560, 1, 5, 1, 0);
        else
            p->shadowId = ScaledShadow_Initialize((u8 *)&p->shadow, &p->render.x, 64, 16, 128, 100,
                                                  2560, 1, 5, 1, flag);
    }
    p->scriptI = Script_GetKeywordValue('I', 0);
    sequence = &p->frames;
    if (p->scriptI) {
        Memory_Clear((u8 *)&p->arguments, 32);
        if (Script_SeekToKeyword('G')) {
            i = 0;
            do {
                u32 offset = i * 4;
                u32 *base = p->arguments;
                u32 *dest = (u32 *)((u32)base + offset);
                *dest = ReadOptional();
            } while (++i <= 2);
        }
    }
    Anim_SetSequence(sequence, pool->resource, 0, 0, p->animationFlags);

    ActorGroup_StepAnimation(p, render, sequence);
    sub_08105194(p, a, b, c);
}
