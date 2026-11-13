#include "global.h"
#include "engine.h"
#include "ram.h"
#include "engine_globals.h"

struct ColorPulse {
    u8 pad0[0xB4C];
    u16 periodB4C;
    u16 timerB4E;
    u8 padB50[0xCE4 - 0xB50];
    void (*updateCE4)(struct ColorPulse *);
    u8 padCE8[0xCF0 - 0xCE8];
    void (*callbackCF0)(struct ColorPulse *);
    u8 padCF4[0xD53 - 0xCF4];
    s8 approaches;
};

s32 Callback_InvokeActor(void *, void *);

static inline u8 ChildGroup_InvokeActorByte(void *actor, void *callback)
{
    return Callback_InvokeActor(actor, callback);
}

void ChildGroup_LaunchNextInactiveMember();

/* Twin of ColorPulse_BeginNearApproach: after two approaches, restart the
   timer and hand over to ChildGroup_LaunchNextInactiveMember. */

void ChildGroup_AdvanceLaunchCycle(struct ColorPulse *p)
{
    if (p->callbackCF0 && ChildGroup_InvokeActorByte(p, p->callbackCF0))
        p->approaches++;
    if (p->approaches > 1) {
        p->approaches = 0;
        p->timerB4E = p->periodB4C;
        p->updateCE4 = ChildGroup_LaunchNextInactiveMember;
    }
}

struct SwordChild {
    u8 pad0[0x29];
    u8 flags;
    u8 pad2A[4];
    u8 active, entered;
    u8 pad30[0x74 - 0x30];
    void (*update)(void);
    u8 pad78[0x194 - 0x78];
};

struct SwordGroup {
    u8 pad0[0x5AC];
    struct SwordChild children[3];
    u8 padA68[0xB4C - 0xA68];
    u16 launchDelay, delay;
    u8 padB50[0xCE4 - 0xB50];
    void (*callback)(struct SwordGroup *);
    u8 padCE8[0xD2C - 0xCE8];
    u8 count;
};

void Sword_BeginRestOrbit(void);

void ChildGroup_WaitForInactiveMembers();

// confidence=low: launches the next resting sword every launchDelay frames; once all are out,

// waits for them to finish.

void ChildGroup_LaunchNextInactiveMember(struct SwordGroup *p)
{
    s32 i;
    u32 stride;
    if (p->delay) {
        p->delay--;
        return;
    }
    i = 0;
    if (i < p->count) {
        stride = sizeof(struct SwordChild);
        for (; i < p->count; i++) {
            if (p->children[i].active == 0) {
                u32 address = i * stride;
                struct SwordChild *child;
                void (**callback)(void);
                u32 offset;
                address += (u32)p;
                address += (u32) & ((struct SwordGroup *)0)->children;
                child = (struct SwordChild *)address;
                {
                    u8 one = 1;
                    u8 *active = &child->active;
                    s16 entered = 0;
                    *active = one;
                    child->entered = entered;
                    child->flags = one | child->flags;
                    offset = i * stride;
                    callback = &p->children[0].update;
                    callback = (void *)((u8 *)callback + offset);
                    *callback = Sword_BeginRestOrbit;
                    p->delay = p->launchDelay;
                    return;
                }
            }
        }
    }

    p->callback = ChildGroup_WaitForInactiveMembers;
}

struct InactiveChildGroupMember {
    u8 active;
    u8 pad[0x193];
} __attribute__((packed));

struct InactiveChildGroup {
    u8 pad0[0x8D];
    u8 flags8D;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[0x546];
    struct InactiveChildGroupMember members[4];
    u8 padC2A[0x9E];
    void (*callbackCC8)(struct InactiveChildGroup *);
    u8 padCCC[0x18];
    void (*callbackCE4)(struct InactiveChildGroup *);
    u8 padCE8[0x40];
    u16 timer;
    u8 padD2A[2];
    u8 count;
};

void ChildGroup_BeginInactiveState();

void ChildGroup_WaitForInactiveMembers(struct InactiveChildGroup *p)
{
    s32 active = 0;
    s32 n;
    struct InactiveChildGroupMember *member;
    if (active < p->count) {
        member = p->members;
        {
            u32 stride = sizeof(struct InactiveChildGroupMember);
            n = p->count;
            do {
                if (member->active)
                    active++;
                member = (struct InactiveChildGroupMember *)((u8 *)member + stride);
            } while (--n != 0);
        }
    }
    if (active == 0) {
        u16 one = 1;
        u8 *q;
        q = &p->state92;
        do {
            *q = one;
        } while (0);
        q++;
        *q = one;
        p->flags8D = one | p->flags8D;
        p->callbackCC8 = ChildGroup_BeginInactiveState;
    } else if (active < p->count) {
        if (++p->timer <= 7)
            p->callbackCE4 = ChildGroup_WaitForInactiveMembers;
    }
}

struct DamageChildGroup {
    u8 pad0[0x7E];
    s16 field7E;
    u8 pad80[0xB04];
    s32 position[3];
    u8 padB90[0xE0];
    u8 display[0xEC];
    s16 fieldD5C;
    u8 fieldD5E;
    u8 padD5F[0x19];
    u16 damage;
    u8 padD7A[2];
    u16 fieldD7C;
    u8 padD7E[8];
    s16 fieldD86;
};

void SceneHud_SetTimedState(u32, s32, s32, u32, u32, s32);

void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

struct DamageChildGroupOffset {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

static inline void CpuSetC(void *dst, u32 control, void *src)
{
    CpuSet(src, dst, control);
}

void ChildGroup_ShowDamageNumber(struct DamageChildGroup *p, u32 flags, s32 mode)
{
    struct {
        u32 fill;
        struct DamageChildGroupOffset offset;
    } scratch;
    struct DamageChildGroupOffset *vector;
    SceneHud_SetTimedState(p->fieldD7C, p->field7E, p->fieldD5C, p->fieldD5E, (flags >> 2) & 1,
                           p->fieldD86);
    vector = &scratch.offset;
    /* fill source is initialized with the call argument. */
    scratch.fill = 0;
    CpuSetC(vector, 0x05000002, &scratch.fill);
    vector->y = -120;
    DamageNumberDisplay_Show(p->display, p->damage, mode, p->position, vector, 1);
    p->damage = 0;
}

struct IvyPaletteBuffers {
    u8 unknown0[0xB0C];
    u16 blended[16];
    u16 source[16];
    u8 unknownB4C[0xBA0 - 0xB4C];
    u16 *sourcePalette;
};

void Ivy_InitPaletteBuffers(struct IvyPaletteBuffers *p)
{
    u16 **sp = &p->sourcePalette;
    u32 z = 0;
    u16 *s = *sp;
    u16 *d = p->blended;
    s32 i = 15;
    do {
        *d = z;
        d[16] = *s;
        s++;
        d++;
        i--;
    } while (i >= 0);
}

void Ivy_SetSegmentFlags(u8 *p, u32 v)
{
    s32 i = 3;
    u32 *q = (u32 *)(p + 0x1b8);
    do {
        *q = v;
        q -= 7;
        i--;
    } while (i >= 0);
}

struct IvySegmentHalfwordView {
    u16 property;
    u8 unknown2[0x1A];
};

struct IvySegmentWordView {
    u32 property;
    u8 unknown4[0x18];
};

void Ivy_SetSegmentProperties(u8 *p, u32 b, s32 a)
{
    struct IvySegmentWordView *word = (struct IvySegmentWordView *)(p + 0x164);
    struct IvySegmentHalfwordView *halfword = (struct IvySegmentHalfwordView *)(p + 0x15e);
    s32 i = 3;
    do {
        halfword->property = a;
        word->property = b;
        word++;
        halfword++;
    } while (--i >= 0);
}

/* Tentative family name: owner of the active child-record group. */

struct ChildGroupParameters {
    u8 pad0[0xB4C];
    u16 parameterB4C;
    u8 padB4E[2];
    u16 parameterB50, parameterB52, parameterB54;
};

s32 Script_GetValue(void);

static inline s32 ReadParameter(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

void ChildGroup_ReadScriptParameters(struct ChildGroupParameters *p)
{
    if (Script_SeekToKeyword('P')) {
        p->parameterB4C = ReadParameter(60);
        p->parameterB50 = ReadParameter(2);
        p->parameterB52 = ReadParameter(80);
        p->parameterB54 = ReadParameter(1);
    } else {
        p->parameterB4C = 60;
        p->parameterB50 = 2;
        p->parameterB52 = 80;
        p->parameterB54 = 1;
    }
}

/* Tentative family name: owner of the active child-record group. */

struct ChildGroupEventStatePosition {
    u32 x, y;
};

struct ChildGroupEventState {
    u8 pad0[0x7E];
    s16 health;
    u8 pad80[0xD];
    u8 flags;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[8];
    u16 flashTimer;
    u8 pad9E[0xAD6];
    void *animation;
    u8 padB78[0xC];
    struct ChildGroupEventStatePosition position;
    u8 padB8C[0x13C];
    void (*callback)(struct ChildGroupEventState *);
    u8 padCCC[0x87];
    u8 fieldD53;
    u8 padD54[4];
    u16 palette;
    u8 padD5A[6];
    u32 fieldD60, fieldD64, fieldD68;
    u8 padD6C[0xC];
    u16 damage, fieldD7A;
    u8 padD7C[0x1C];
    struct ChildGroupEventStatePosition savedPosition;
    u32 hitTimer;
    u8 padDA4[0xC];
};

struct ChildGroupEventStateEvent {
    u8 pad0[0x40];
    u16 timer;
    u8 pad42[0xE];
    struct ChildGroupEventState *owner;
};

void ChildGroup_UpdateTransitionJitter();

void ChildGroup_UpdateLandingState();

static inline void SetState(struct ChildGroupEventState *p, u8 one, u8 id)
{
    p->state92 = one;
    p->state93 = id;
    p->flags |= one;
}

s32 ChildGroup_ProcessEvent(struct ChildGroupEventStateEvent *event, s32 kind, s32 damage)
{
    struct ChildGroupEventState *p = event->owner;
    if (!p || p->health <= 0)
        return 0;
    switch (kind) {
        case 0:
            event->timer = 0;
            p->hitTimer = 10;
            if (damage) {
                p->damage = damage;
                p->fieldD7A = 1;
                p->fieldD68 = 1;
                p->fieldD64 = 0;
                p->fieldD60 = 0;
                p->flashTimer = 8;
                AnimData_SelectPaletteBank(p->animation, p->palette);
            } else {
                p->damage = 0;
                p->fieldD7A = 0;
                p->fieldD68 = 0;
                p->fieldD64 = 0;
                p->fieldD60 = 0;
                p->callback = ChildGroup_UpdateTransitionJitter;
                SetState(p, 1, 7);
            }
            break;
        case 1:
            p->fieldD53 = 0;
            p->callback = ChildGroup_UpdateLandingState;
            p->position = p->savedPosition;
            SetState(p, 1, 4);
            break;
    }
    return 1;
}

/* Tentative family name: owner of the active child-record group. */

#define ChildGroup_UpdateVerticalOscillation Ivy_UpdateBobbing

struct ChildGroupOscillationState {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[4];
    u8 state92, state93;
    u8 pad94[0xE];
    u8 reset;
    u8 padA3[0xC25];
    void (*callbackCC8)(struct ChildGroupOscillationState *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct ChildGroupOscillationState *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct ChildGroupOscillationState *);
    u32 timer;
};

void sub_080F1FE8();

void Ivy_UpdateBobbing();


void Ivy_WaitForPrimaryActionCompletion();

static inline u8 EnterState_3848(struct ChildGroupOscillationState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

static inline void SetState_080F3848(struct ChildGroupOscillationState *p, u8 one, u8 id)
{
    p->state92 = one;
    p->state93 = id;
    p->flags |= one;
}

void ChildGroup_UpdateTimedOscillation(struct ChildGroupOscillationState *p)
{
    if (EnterState_3848(p)) {
        p->timer = 150;
        p->callbackCF0 = sub_080F1FE8;
    }
    Ivy_UpdateBobbing(p);
    if (--p->timer == 0) {
        SetState_080F3848(p, 1, 4);
        p->callbackCC8 = ChildGroup_UpdateLandingState;
        p->callbackCD8 = Ivy_WaitForPrimaryActionCompletion;
    }
}

struct CGPos080F38E0 {
    u16 x, y, z, w;
};

struct CGKid080F38E0 {
    u32 flags;
    u8 pad4[2];
    u8 b6;
    u8 pad7[0x25];
};

struct CGSrc080F38E0 {
    u8 pad0[0x30];
    u16 h30, h32, h34;
};

struct Group080F38E0 {
    u8 pad0[0x60];
    struct CGSrc080F38E0 *src60;
    u8 pad64[0x29];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[5];
    struct CGKid080F38E0 kids[4];
    u8 pad158[0x330];
    u16 f488[3];
    u8 pad48E[2];
    u16 f490[3];
    u8 pad496[2];
    u16 f498[3];
    u8 pad49E[0x12];
    u8 b4B0;
    u8 pad4B1[3];
    u16 h4B4;
    u8 pad4B6[0x6B8];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos080F38E0 pos;
    u8 padB8C[0x13C];
    void (*callbackCC8)(struct Group080F38E0 *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct Group080F38E0 *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct Group080F38E0 *);
    u8 padCF4[8];
    u32 fieldCFC;
    u8 padD00[0x53];
    u8 fieldD53;
    u8 padD54[0x44];
    struct CGPos080F38E0 saved;
    s32 counterDA0;
};

void Ivy_InitColorTransition();


static inline u8 ChildGroup_TakeEntryFlag(struct Group080F38E0 *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

static inline u32 ChildGroup_Jitter(u32 value, const u16 *base)
{
    u32 mask = 31;
    value &= mask;
    return value + *base - 15;
}

static inline u32 ChildGroup_RandomValue(void)
{
    const u16 *table = gRandomTable;
    u32 *index = &gRandomIndex;
    *index = (*index + 1) & 1023;
    return table[*index];
}

// scope shape 52 (differences once low registers are set aside)


void ChildGroup_UpdateTransitionJitter(struct Group080F38E0 *p)
{
    s32 i;

    // confidence=low: per-frame update of the child group: resets kid flags, snapshots
    // position, cycles the random table into the position, then runs the callback.
    if (ChildGroup_TakeEntryFlag(p)) {
        p->f498[0] = p->pos.x;
        p->f498[1] = p->pos.y;
        p->f498[2] = p->pos.z;
        p->f488[0] = p->src60->h30;
        p->f488[1] = 0x15E + p->src60->h32;
        p->f488[2] = p->src60->h34;
        p->f490[0] = p->f488[0] - p->pos.x;
        p->f490[1] = p->f488[1] - p->pos.y;
        p->f490[2] = p->f488[2] - p->pos.z;
        for (i = 0; i < 4; i++) {
            p->kids[i].b6 = 0x80;
            p->kids[i].flags |= 1;
        }
        p->b4B0 = 0;
        p->h4B4 = 0;
        p->saved = p->pos;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
    }
    if (p->counterDA0 > 0) {
        p->pos.x = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.x);
        p->pos.y = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.y);
        p->pos.z = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->saved.z);
        p->counterDA0--;
    }
    p->fieldCFC = 0;
    if (p->callbackCF0 != 0) {
        Callback_InvokeActor(p, p->callbackCF0);
    }
}

struct ChildGroupInactiveState {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0xC25];
    void (*callbackCC8)(struct ChildGroupInactiveState *);
    u8 padCCC[8];
    void (*callbackCD4)(struct ChildGroupInactiveState *);
    u8 padCD8[0x18];
    void (*callbackCF0)(struct ChildGroupInactiveState *);
    u32 timer;
};



void Ivy_WaitForRetarget(struct ChildGroupInactiveState *);


static inline u8 EnterState_3ABC(struct ChildGroupInactiveState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

void ChildGroup_BeginInactiveState(struct ChildGroupInactiveState *p)
{
    if (EnterState_3ABC(p)) {
        p->timer = 100;
        p->callbackCD4 = Ivy_WaitForRetarget;
        p->callbackCF0 = sub_080F1FE8;
    }
    Ivy_UpdateBobbing(p);
    if (p->callbackCD4)
        Callback_InvokeActor(p, p->callbackCD4);
}

struct ChildGroupTrailDelay {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0xC3A];
    void (*callbackCC8)(struct ChildGroupTrailDelay *);
    u8 padCCC[0xB4];
    s32 elapsedD80;
};

void ChildGroup_UpdateTrailingMembers();

/* After 30 frames, hand over to ChildGroup_UpdateTrailingMembers and set
   flag bit 0. The bit is a local because the ROM loads it before the
   flag's address. */

void ChildGroup_WaitForTrailingMembers(struct ChildGroupTrailDelay *p)
{
    if (p->elapsedD80 > 30) {
        p->callbackCC8 = ChildGroup_UpdateTrailingMembers;
        {
            u8 bit = 1;
            p->flags |= bit;
        }
    }
}

/* Tentative family name: owner of the active child-record group. */

struct CGPos {
    u16 x, y, z, w;
};

struct CGKid {
    u32 flags;
    u8 pad4[2];
    u8 b6;
    u8 pad7[0x15];
    u16 x, y, z;
    u8 pad[0xA];
};

struct ChildGroupTrailingState {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[5];
    struct CGKid kids[4];
    u8 pad158[0x358];
    u8 b4B0;
    u8 pad4B1[3];
    u16 h4B4;
    u8 pad4B6[0x6B8];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x13C];
    void (*callbackCC8)(struct ChildGroupTrailingState *);
    u8 padCCC[0xC];
    void (*callbackCD8)(struct ChildGroupTrailingState *);
    u8 padCDC[0x14];
    void (*callbackCF0)(struct ChildGroupTrailingState *);
    u8 padCF4[0x5C];
    u32 fieldD50;
    u8 padD54[6];
    u16 hD5A;
    u8 padD5C[0x24];
    u32 fieldD80;
};



void Sound_PlaySong(u32);

static inline u8 EnterState_3B60(struct ChildGroupTrailingState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

// Differs only in where `movs r2, #3` (the loop counter) is placed: the ROM sets it before the kid-0 pointers.

void ChildGroup_UpdateTrailingMembers(struct ChildGroupTrailingState *p)
{
    if (EnterState_3B60(p)) {
        int i;
        for (i = 0; i < 4; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
        }

        for (i = 0; i < 4; i++)
            p->kids[i].flags &= ~1;

        p->callbackCD8 = Ivy_WaitForPrimaryActionCompletion;
        p->h4B4 = 0;
        p->b4B0 = 0;
        p->hD5A = 0;
        p->callbackCF0 = Ivy_InitColorTransition;
        *(((u8 *)p) + 0xD53) = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->callbackCD8)
        Callback_InvokeActor(p, p->callbackCD8);
    if (p->fieldD80 & 1) {
        int i;
        u16 *x0;
        u16 *y0;
        u16 *z0;
        u8 *b0;
        // A loop around the counter's start doubles its use weight in global
        // allocation (flow weights refs by loop depth): i gets r2 as in the ROM.
        do {
            i = 3;
        } while (0);
        x0 = &p->kids[0].x;
        y0 = &p->kids[0].y;
        z0 = &p->kids[0].z;
        b0 = &p->kids[0].b6;
        for (; i > 0; i--) {
            p->kids[i].x = p->kids[i - 1].x;
            p->kids[i].y = p->kids[i - 1].y;
            p->kids[i].z = p->kids[i - 1].z;
            p->kids[i].b6 = p->kids[i - 1].b6;
        }

        *x0 = p->pos.x;
        *y0 = p->pos.y;
        *z0 = p->pos.z;
        *b0 = p->bB6E;
    }
}

/* Tentative family name: owner of the active child-record group. */

struct ChildGroupOrbitState;

struct ChildGroupOrbitState {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0x5];
    struct CGKid kids[4];
    u8 pad158[0x348];
    s32 angle[4];
    s8 speed;
    u8 b4B1;
    u8 pad4B2[0x6];
    s32 radius;
    u8 pad4BC[0x694];
    u16 count;
    u8 padB52[0x1C];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x150];
    void (*callbackCDC)(struct ChildGroupOrbitState *);
    u8 padCE0[0x10];
    void (*callbackCF0)(struct ChildGroupOrbitState *);
    u8 padCF4[0x5F];
    u8 fieldD53;
    u8 padD54[0x30];
    u16 timer;
};

void Ivy_WaitForSecondaryActionCompletion(struct ChildGroupOrbitState *);



s32 Div(s32, s32);

static inline u8 EnterState_3CC4(struct ChildGroupOrbitState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

static inline s32 truncateShift(s32 value, s32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

void ChildGroup_UpdateOrbitingMembers(struct ChildGroupOrbitState *p)
{
    int i;
    if (EnterState_3CC4(p)) {
        for (i = 0; i < p->count; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
            p->kids[i].flags &= ~1;
            p->angle[i] = Div(0x100, p->count) * i;
        }
        p->radius = 0;
        p->b4B1 = 0;
        p->speed = 5;
        p->timer = 0;
        for (i = 0; i < p->count; i++) {
            u32 rec = (u32)p + i * 0x54;
            *(u16 *)(rec + 0x1CE) &= ~4;
            *(u16 *)(rec + 0x31E) &= ~4;
        }
        p->callbackCDC = Ivy_WaitForSecondaryActionCompletion;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
        Sound_PlaySong(0x16F);
    }
    Ivy_UpdateBobbing(p);
    i = 0;
    if (i < p->count) {
        s32 *radius = &p->radius;
        const s16 *cosT = gUnk_086149C4;
        const s16 *sinT = cosT;
        s32 *a = p->angle;
        do {
            p->kids[i].x = p->pos.x + truncateShift(cosT[(*a + 64) & 255] * *radius, 10);
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z + truncateShift(sinT[*a & 255] * *radius, 10);
            *a += p->speed;
            if (*a > 255) {
                if (p->timer > 0x46)
                    Sound_PlaySong(0x169);
                *a -= 255;
            }
            a++;
            i++;
        } while (i < p->count);
    }
    if (p->callbackCDC)
        Callback_InvokeActor(p, p->callbackCDC);
    p->timer++;
}

/* Tentative family name: owner of the active child-record group. */

struct ChildGroupLandingState;

struct ChildGroupLandingState {
    u8 pad0[0x64];
    u16 h64;
    u8 pad66[0x2];
    u16 h68;
    u8 pad6A[0x2];
    u16 h6C;
    u8 pad6E[0x2];
    u16 h70;
    u8 pad72[0x1B];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0x5];
    struct CGKid kids[4];
    u8 pad158[0xA10];
    u32 fieldB68;
    u8 padB6C[0x2];
    u8 bB6E;
    u8 padB6F[0x15];
    struct CGPos pos;
    u8 padB8C[0x96];
    u16 hC22;
    u8 padC24[0x52];
    u16 hC76;
    u8 padC78[0x68];
    void (*callbackCE0)(struct ChildGroupLandingState *);
    u8 padCE4[0xC];
    void (*callbackCF0)(struct ChildGroupLandingState *);
    u8 padCF4[0xC];
    u32 fieldD00;
    u8 padD04[0x4F];
    u8 fieldD53;
};

s32 Terrain_GetHeightAt(void *);

void sub_080F2F24(struct ChildGroupLandingState *);


static inline u8 EnterState_3F48(struct ChildGroupLandingState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

void ChildGroup_UpdateLandingState(struct ChildGroupLandingState *p)
{
    if (EnterState_3F48(p)) {
        int i;
        p->fieldB68 |= 1;
        p->hC22 |= 4;
        p->hC76 |= 4;
        for (i = 0; i < 4; i++) {
            p->kids[i].x = p->pos.x;
            p->kids[i].y = p->pos.y;
            p->kids[i].z = p->pos.z;
            p->kids[i].b6 = p->bB6E;
            p->kids[i].flags &= ~1;
            {
                u32 rec = (u32)p + i * 0x54;
                *(u16 *)(rec + 0x1CE) |= 4;
                *(u16 *)(rec + 0x31E) |= 4;
            }
            p->fieldD00 = (u16)Terrain_GetHeightAt(&p->pos);
        }
        p->h64 = 0;
        p->h68 = 0;
        p->h6C = -7;
        p->h70 = -7;
        p->callbackCE0 = sub_080F2F24;
        p->callbackCF0 = Ivy_InitColorTransition;
        p->fieldD53 = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->callbackCE0)
        Callback_InvokeActor(p, p->callbackCE0);
}

#define gChildGroupRandomIndex               gRandomIndex

struct ChildGroupCallbackState {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0xC41];
    void (*callbackCE4)(struct ChildGroupCallbackState *);
    void (*callbackCE8)(struct ChildGroupCallbackState *);
    u8 padCEC[0x3C];
    u16 timerD28, timerD2A;
    u8 count, randomByte;
    u8 padD2E[0x27];
    u8 mode;
};

void ChildGroup_AdvanceLaunchCycle();

void Sword_UpdateSpinningLungeAttack(struct ChildGroupCallbackState *);

void Sword_UpdateThrustAttack(struct ChildGroupCallbackState *);

void Sword_UpdateWaveAttack(struct ChildGroupCallbackState *);


static inline u8 EnterState_40C8(struct ChildGroupCallbackState *p)
{
    u32 mask = 1;
    u32 flags = p->flags;
    if (flags & mask) {
        u32 cleared = ~1;
        cleared &= flags;
        {
            u32 zero = 0;
            p->flags = cleared;
            p->reset = zero;
        }
        return 1;
    }
    return 0;
}

void ChildGroup_UpdateAlternatingCallbacks(struct ChildGroupCallbackState *p)
{
    if (EnterState_40C8(p)) {
        p->timerD28 = 0;
        {
            u32 random = (gRandomIndex + 1) & 0x3FF;
            u32 value;
            const u16 *table;
            gRandomIndex = random;
            random <<= 1;
            table = (u16 *)0x0203B400;
            value = *(u16 *)(random + (u32)table);
            p->randomByte = value;
        }
        p->callbackCE4 = ChildGroup_AdvanceLaunchCycle;
        p->timerD2A = 0;
        switch (p->mode) {
            case 0:
                p->callbackCE8 = Sword_UpdateSpinningLungeAttack;
                break;
            case 1:
                p->callbackCE8 = Sword_UpdateThrustAttack;
                break;
        }
        Sound_PlaySong(0x16F);
    }
    Ivy_UpdateBobbing(p);
    if (p->callbackCE4) {
        Callback_InvokeActor(p, p->callbackCE4);
    }
    if ((++p->timerD2A & 0x3FF) == 0) {
        p->timerD2A = 0;
        switch (p->mode) {
            case 0:
                p->callbackCE8 = Sword_UpdateSpinningLungeAttack;
                break;
            case 1:
                if (p->callbackCE8 == Sword_UpdateWaveAttack) {
                    p->callbackCE8 = Sword_UpdateThrustAttack;
                } else if (p->callbackCE8 == Sword_UpdateThrustAttack) {
                    p->callbackCE8 = Sword_UpdateWaveAttack;
                }
                break;
        }
    }
}
