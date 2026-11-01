#include "contact_swarm.h"
#include "display_sequence_inline.h"

extern void *gUnknown_02000580;
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 Terrain_GetHeightAt(void *);
void Ivy_InitPaletteBuffers(void *);
void Ivy_SetSegmentProperties(void *, void *, u32);
void ChildGroup_ReadScriptParameters(void *);
void TargetRegistry_RegisterSceneActor(void *, void *, void *);
void Ivy_OnDestroy(void);
void ColorPulse_ApplyContact(void);
void ChildGroup_ProcessEvent(void);
void sub_080F20AC(void);
void sub_080F3B30(void);
void Ivy_InitColorTransition(void);

union SwordVector {
    struct ContactPackedVector packed;
    struct ContactPosition position;
};

struct SwordTask {
    u8 unknown0[0x60];
    void *target;
    u16 field64, field66, field68, field6A, field6C, field6E, field70;
    u8 unknown72[0xC];
    s16 health, body80, field82, field84, field86, body88, body8A;
    u8 field8C, flags8D, field8E, field8F, field90, field91, field92, field93;
    u8 unknown94[0x6];
    u16 field9A, field9C;
    u8 field9E, field9F;
    u8 unknownA0[0x8];
    struct ContactSwarmSprite segments[4];
    struct ContactSwarmGraphics segmentGraphics[4];
    struct ContactCollider segmentAttacks[4];
    struct ContactCollider segmentBodies[4];
    u8 unknown468[0x6F0];
    struct DisplaySequenceState sequence;
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
    u8 shadow[0x6C];
    struct ContactCollider attack;
    struct ContactCollider body;
    u8 unknownCC4[0x4];
    void *update;
    u8 unknownCCC[0x4];
    void *onContact;
    u8 unknownCD4[0x18];
    u32 fieldCEC;
    void *colorUpdate;
    u8 unknownCF4[0x4];
    u32 duration;
    u8 unknownCFC[0x8];
    s16 attackD04[2], attackD08[2], attackD0C[2], attackD10[2];
    u8 unknownD14[0x1C];
    u16 palette[16];
    u8 unknownD50[0x2];
    u8 fieldD52, fieldD53, fieldD54, variant;
    u16 paletteBank;
    u8 unknownD58[0x4];
    u16 initialHealth;
    u8 parameter;
    u8 unknownD5F[0x27];
    u16 fieldD86;
    u8 registry[4];
};

static inline s32 Sword_ReadArgument(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}

static inline void Sword_SetFrame(struct ContactSwarmSprite *sprite, u16 frame)
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
static inline u32 Sword_StepSequence(struct ContactSwarmSprite *owner,
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
static inline u16 Sword_SequenceIndex(struct DisplaySequenceState *const *state)
{
    return (*state)->index;
}
static inline void Sword_HoldElapsed(struct DisplaySequenceState *state, u16 duration)
{
    state->elapsed = duration;
}
static inline u32 Sword_StepMainSequence(struct ContactSwarmSprite *owner,
                                         struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    u32 duration;
    u16 heldDuration;
    u16 elapsedSnapshot[1];
    u32 elapsedValue;
    {
        u32 offset = Sword_SequenceIndex(&state) << 1;
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
    /* Retain the stored counter and the duration of the current frame. */
    elapsedSnapshot[0] = ++state->elapsed;
    duration = state->duration;
    elapsedValue = elapsedSnapshot[0];
    heldDuration = duration;
    if (elapsedValue >= duration) {
        u32 flags;
        state->elapsed = 0;
        flags = state->flags;
        if (flags & 4) {
            if (state->index == 0) {
                u32 holdMask = 8;
                holdMask &= flags;
                if (holdMask)
                    Sword_HoldElapsed(state, heldDuration);
                else
                    Sword_SetFrameIndex(state, Sword_GetFrameCount(state) - 1);
                ended = 1;
            } else {
                Sword_SetFrameIndex(state, state->index - 1);
                ended = 0;
            }
        } else {
            u8 frameCount;
            if (++state->index >= (frameCount = Sword_GetFrameCount(state))) {
                u32 holdMask = 8;
                holdMask &= flags;
                if ((u8)holdMask) {
                    state->elapsed = duration;
                    Sword_SetFrameIndex(state, frameCount - 1);
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

static inline void Sword_SetByteFlags(u8 *field, u8 bits)
{
    bits |= *field;
    *field = bits;
}

static inline void *Sword_ReadSpriteResource(void *const *field)
{
    return *field;
}
static inline void Sword_InitSegmentCollider(struct ContactCollider *collider, u32 flags,
                                             const union SwordVector *halfSize,
                                             const union SwordVector *offset,
                                             const u32 *contactFlags)
{
    Collider_Init(collider, 0, flags, 0, *contactFlags, halfSize, offset);
}
static inline void Sword_ReadArgumentField(s16 *array, s32 index)
{
    s16 *field = (s16 *)((u8 *)array + index);
    *field = Sword_ReadArgument();
}
/* Initialize the main actor and its four child sprites from the script. */
void Sword_InitTask(struct SwordTask *p)
{
    struct DisplaySequenceState *anim;
    struct ContactSwarmSprite *spriteMain;
    struct ContactSwarmGraphics *animMain;
    union SwordVector hit;
    union SwordVector box;
    s32 i;
    s32 sb;

    p->target = gUnknown_02000580;
    animMain = &p->graphics;
    spriteMain = &p->sprite;
    p->field64 = 0;
    p->field66 = 0;
    p->field68 = 0;
    p->field6C = 0;
    p->field6E = 1;
    p->field70 = 0;
    p->flags8D = 0;
    p->health = 0;
    p->field8E = 0;
    p->field9E = 0;
    p->field9F = 0;
    p->field9A = 0;
    p->field9C = 0;
    p->field92 = 1;
    p->field93 = 0;
    Sword_SetByteFlags(&p->flags8D, 1);
    p->update = sub_080F3B30;
    p->colorUpdate = Ivy_InitColorTransition;
    p->fieldCEC = 0;
    p->duration = 0x14;

    AnimData_Load(animMain, 0x5291);
    Sprite_Init(spriteMain, animMain, 0);
    {
        void *script = ScriptProc_Start(0x922E, 0x3F72);
        anim = &p->sequence;
        Anim_SetSequence(anim, script, 0, p->variant, 0);
    }

    Sword_StepMainSequence(spriteMain, anim);
    spriteMain->flags |= 2;

    AnimData_SelectPaletteBank(
        Sword_ReadSpriteResource(&((struct DisplaySequenceSprite *)&p->sprite)->resource),
        p->paletteBank);

    {
        for (i = 0; i < 16; i++) {
            *(p->palette + i) = ((const u16 *)animMain->paletteData)[i];
        }
        animMain->paletteData = p->palette;
    }
    p->fieldD52 = 5;
    p->fieldD53 = 0;
    p->fieldD54 = 0;

    ScaledShadow_Initialize(p->shadow, &p->sprite.position, 0x30, 0x30, 0x30, 0x30, 0, 0, 0, 0, 0);

    if (Script_SeekToKeyword(0x41)) {
        p->health = Sword_ReadArgument();
        p->body80 = Sword_ReadArgument();
        p->field82 = Sword_ReadArgument();
        p->field84 = Sword_ReadArgument();
        p->body8A = Sword_ReadArgument();
        p->body88 = Sword_ReadArgument();
        p->initialHealth = p->health;
    }
    if (Script_SeekToKeyword(0x54)) {
        for (sb = 0, i = 1; i >= 0; sb += sizeof(s16), i--) {
            Sword_ReadArgumentField(p->attackD04, sb);
            Sword_ReadArgumentField(p->attackD08, sb);
            Sword_ReadArgumentField(p->attackD0C, sb);
            Sword_ReadArgumentField(p->attackD10, sb);
        }
    }
    if (Script_SeekToKeyword(0x70)) {
        spriteMain->position.x = Script_GetValue();
        spriteMain->position.y = Script_GetValue();
        spriteMain->position.z = Script_GetValue();
    } else {
        spriteMain->position.x = 0;
        spriteMain->position.y = 0;
        spriteMain->position.z = 0;
    }
    p->parameter = Script_GetKeywordValue(0x4B, 2);
    p->fieldD86 = Script_GetKeywordValue(0x56, -1);
    spriteMain->position.y = Terrain_GetHeightAt(&spriteMain->position) + 0x15E;

    hit.packed.x = 50;
    hit.packed.y = 50;
    hit.packed.z = 50;
    box.packed.x = 0;
    box.packed.y = 0;
    box.packed.z = 0;

    Collider_Init(&p->attack, 0, 0x2001, 0, 0x10, &hit, &box);
    Collider_SetAttackProperties(&p->attack, p->attackD04[0], p->attackD08[0], 0, p->attackD0C[0],
                                 p->attackD10[0]);
    Collider_SetCallback(&p->attack, Ivy_OnDestroy, p);
    Collider_Refresh(&p->attack);
    p->attack.flags &= 0xFFFB;

    hit.position.x = 0x40;
    hit.position.y = 0x40;
    hit.position.z = 0x40;
    Collider_Init(&p->body, 0, 0x5009, 0, 0x10, &hit, &box);
    Collider_SetBodyProperties(&p->body, p->body80, p->body8A, p->body88);
    Collider_SetCallback(&p->body, ColorPulse_ApplyContact, p);
    Collider_Refresh(&p->body);
    p->onContact = sub_080F20AC;

    {
        u32 childContactFlags = 0x10;
        for (i = 0; i <= 3; i++) {
            struct ContactSwarmSprite *sprite;
            u32 spriteOffset;
            struct ContactCollider *attack;
            struct ContactCollider *body;
            struct DisplaySequenceState *anim2 = &p->sequence;
            struct ContactSwarmGraphics *animSeg = &p->segmentGraphics[i];
            AnimData_Load(animSeg, 0x5291);
            spriteOffset = i * sizeof(struct ContactSwarmSprite);
            Sprite_Init(&p->segments[i], animSeg, 0);
            sprite = (struct ContactSwarmSprite *)(spriteOffset + (u32)p);
            sprite = (struct ContactSwarmSprite *)((u8 *)sprite + 0xA8);
            Sword_StepSequence(sprite, anim2);

            p->segments[i].flags = 0x203;
            p->segments[i].priority = 3;
            p->segments[i].position.x = 0;
            p->segments[i].position.y = 0;
            p->segments[i].position.z = 0;

            attack = &p->segmentAttacks[i];
            Sword_InitSegmentCollider(attack, 0x2001, &hit, &box, &childContactFlags);
            Collider_SetCallback(attack, 0, p);
            Collider_Refresh(attack);
            Collider_SetAttackProperties(attack, p->attackD04[1], p->attackD08[1], 0,
                                         p->attackD0C[1], p->attackD10[1]);

            p->segmentAttacks[i].flags |= 4;
            body = &p->segmentBodies[i];
            Sword_InitSegmentCollider(body, 0x4009, &hit, &box, &childContactFlags);
            Collider_SetBodyProperties(body, p->body80, p->body8A, p->body88);
            Collider_SetCallback(body, 0, p);
            Collider_Refresh(body);
            p->segmentBodies[i].flags |= 4;
        }
    }

    Ivy_InitPaletteBuffers(p);
    Ivy_SetSegmentProperties(p, (u8 *)p + 0xB0C, 0x13C);
    ChildGroup_ReadScriptParameters(p);
    Script_GetKeywordValue(0x4E, 0);
    if (Script_GetKeywordValue(0x4D, 0) & 0x200)
        TargetRegistry_RegisterSceneActor(p->registry, &p->body, ChildGroup_ProcessEvent);
}
