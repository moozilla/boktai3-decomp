#include "global.h"
#include "actor_graphics_and_basic_states.h"
#include "contact_actor.h"

int SceneActorGraphics_GetResourceId(struct StateActor *);

extern u8 *gUnk_02000580;
extern u32 gUnk_030053F4;
void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void Sound_PlaySongChecked(u32);
void DamageNumberDisplay_Show(void *, u32, u32, void *, void *, u32);
s32 Math_Atan2(s32, s32);
s32 WirelessGame_TryAddCyclingItem(void *);
void WirelessGame_AddItem(void *, s32);
struct ContactVector {
    s16 x, y, z;
};
struct DamageContactActor {
    u8 pad[0x33c];
    union {
        u32 value;
        u16 halves[2];
    } damage;
};

static inline u32 TestContactFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

typedef struct {
    s16 x, y, z;
} Vec;

void SceneActorGraphics_InitBodyNoOp(void);

extern u32 gUnk_030053F4;

void sub_08021F4C(u8 *, u8 *, u8 *);

/* Raw offsets throughout: routing any of this through struct StateActor /
 * SceneActorGraphics changes register allocation enough to break the match
 * (tried: `p->actionFlags`, `p->bodyDrawable`, struct-field attack properties). */

/* A flat `pad[0x344]` prefix (matching groundEffect's offset) rather than
 * struct StateActor + a nested SceneActorGraphics cast: the two-step access
 * changes register allocation enough to break the match. Fields are
 * SceneActorGraphics's scale/scaleMin/scaleMax/scaleStep (0x344-0x34B). */

typedef struct {
    u8 pad[0x344];
    s16 scale, min, max, step;
} Actor;

extern u16 gUnk_02000590;

typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct {
        u32 x, y;
    } pos;
    u8 pad5c[0x4c];
    u8 facing, facing8, padAA[2], mode, state, phase, finished, enter;
    u8 padB1[0x13];
    u32 elapsed;
    u8 padC8[4];
    u8 requestedAngle;
    u8 padCD[0x2df];
    u32 graphicsMode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);

void ContactActorPool_RequestClear(struct ContactActorPool *);

void SceneActors_RequestContactPoolClear(void);

// confidence=low: index of the first of the count tracked positions at 0x110 that lies inside
// the box min..max on x and z, or -1.
struct BoxSearchPosition {
    s16 x, y, z;
};

static inline s32 ReadPositionReference(u32 *entry, struct BoxSearchPosition **out)
{
    *out = (struct BoxSearchPosition *)*entry;
    return *out != NULL;
}

// MATCHING_CONCESSION: register-pinning
// Finds the first tracked position inside the inclusive x/z bounds.
s32 sub_0817A354(void *actor, s32 count, struct BoxSearchPosition *min,
                 struct BoxSearchPosition *max)
{
    u8 *bytes = actor;
    s32 i;

    i = 0;
    if (i >= count)
        goto end;
loop: {
    s32 offset = i * 4;
    struct BoxSearchPosition *pos;
    u32 address = (u32)(bytes + 0x110);

    address += offset;
    ReadPositionReference((u32 *)address, &pos);
    if (pos) {
        s32 x = pos->x;
        s32 minX = min->x;
        register s32 inside asm("r4") = 0;

        if (x >= minX && x <= max->x && pos->z >= min->z && pos->z <= max->z)
            inside = 1;
        if (inside)
            return i;
    }
}
    i++;
    if (i < count)
        goto loop;
end:
    return -1;
}

struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

extern u32 gUnk_03005308;

extern const u8 gUnk_08604DC4[], gUnk_08604DE4[], gUnk_08604E04[], gUnk_08604E24[];

void SceneActorState_StepTableByte(struct StateActor *p, const u8 *tbl);

// confidence=low: chooses the next action phase from the reaction kind and flash state.
// MATCHING_CONCESSION: register-pinning
static inline s32 phaseValue(u8 *p)
{
    s32 value = *p;
    return value;
}

// confidence=low: reaction-kind selector controls the far-point mode and motion timer.
// MATCHING_CONCESSION: register-pinning
void Path_SnapToRandomFarPoint(void *, void *, u32, u32);
struct SnapVector {
    u32 a, b;
};

static inline u8 TakeFlag(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

static inline u8 chk_08023638(struct StateActor *p)
{
    u8 *q = &p->actionEnter;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_080236C0(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_08023720(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_08023770(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets throughout the recovery block below (actionMode 0xAC,
 * actionIndex 0xAD, actionPhase 0xAE, actionTimer 0xC4, actionFinished 0xAF):
 * struct-field writes for that cluster pick a different (still correct, but
 * non-matching) register for the last store. */

struct S_080237C0 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_080237C0(struct S_080237C0 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets in the recovery block: see SceneActorState_Resume for why. */

struct S_0802383C {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_0802383C(struct S_0802383C *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_080238A8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_080238F8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets: see SceneActorState_Resume for why the triplet write needs this shape. */

struct S_08023948 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1[4];
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 padC8[0x2cc];
    u8 particle;
};

static inline u8 chk_08023948(struct S_08023948 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_080239A8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk_080239F8(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

static inline u8 chk(struct StateActor *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023B38 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023B38(struct S_08023B38 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023BE4 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023BE4(struct S_08023BE4 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

/* Raw offsets: see SceneActorState_Resume. */

struct S_08023CA0 {
    u8 f0[0xaa];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
};

static inline u8 chk_08023CA0(struct S_08023CA0 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

extern const u32 gUnk_08604E44[];

/* Raw offsets throughout: the state()/action() helpers each write several
 * adjacent fields together (callback+state+timer+changed+reset+pending, or
 * active+actionReset+action+actionTimer+entered); going through
 * struct StateActor's named fields for those clusters breaks the match
 * (see SceneActorState_Resume). */

struct Common {
    u8 pad0[0x90];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[2];
    u8 healthCheck, pad9b;
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 facing, facing8, requested, state;
    u8 active, actionReset, action, finished, entered, changed;
    u8 padB2[0x12];
    u32 actionTimer;
    u8 padC8[0x38];
    void (*callback)(void *);
};

static inline void state(struct Common *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}

int GridAttack_SelectNextAction(void *);

void sub_08027214(void *);

static inline u8 chk_08023D78(struct Common *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

static inline void action(struct Common *p, int one, int id, int zero)
{
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}

/* Raw offsets: the ending phase/elapsed/enter_08023E24 triplet write breaks the match
 * when routed through struct StateActor's named fields (see SceneActorState_Resume). */

typedef struct State_08023E24 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023E24, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023E24;

typedef struct Actor_08023E24 {
    State_08023E24 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023E24;

static inline u8 enter_08023E24(State_08023E24 *p)
{
    if (p->enter_08023E24) {
        p->enter_08023E24 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void sub_08022EA4(u8 *, struct SnapVector *);

typedef struct State_08023EC0 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023EC0, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023EC0;

typedef struct Actor_08023EC0 {
    State_08023EC0 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023EC0;

static inline u8 enter_08023EC0(State_08023EC0 *p)
{
    if (p->enter_08023EC0) {
        p->enter_08023EC0 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void sub_08022DBC(u8 *);

typedef struct State_08023F34 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_08023F34, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023F34;

static inline u8 enter_08023F34(State_08023F34 *p)
{
    if (p->enter_08023F34) {
        p->enter_08023F34 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

typedef struct State_08023F98 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp;
    s16 maxHp;
    u16 extra;
    u8 vulnerable;
    u8 kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2;
    u8 unknownA3;
    u32 unknownA4;
    u8 facing;
    u8 facing8;
    u8 animation;
    u8 state;
    u8 mode;
    u8 submode;
    u8 phase;
    u8 finished;
    u8 enter_08023F98;
    u8 changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_08023F98;

typedef struct Actor_08023F98 {
    State_08023F98 base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor_08023F98;

static inline u8 enter_08023F98(State_08023F98 *p)
{
    if (p->enter_08023F98) {
        p->enter_08023F98 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

struct S_0802429C {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_0802429C(struct S_0802429C *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_08024334 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024334(struct S_08024334 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_08024440 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024440(struct S_08024440 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_080244D8 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080244D8(struct S_080244D8 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_08024570 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024570(struct S_08024570 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_0802461C {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_0802461C(struct S_0802461C *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

typedef struct State_080246C8 {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter_080246C8, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State_080246C8;

typedef struct Actor_080246C8 {
    State_080246C8 base;
    u8 pad104[0x2A0];
    s32 f3a4;
    u8 pad3a8[0x630];
    void *pool;
} Actor_080246C8;

void ContactActorPool_Start();

static inline u8 enter_080246C8(State_080246C8 *p)
{
    if (p->enter_080246C8) {
        p->enter_080246C8 = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

struct S_080247AC {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080247AC(struct S_080247AC *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

// confidence=low: variant-dependent recovery animations and timed return to the default action.
// MATCHING_CONCESSION: register-pinning
struct VariantRecoveryActor {
    u8 pad0[0xaa];
    u8 animation, state, mode, index, phase, finished, enter;
    u8 padb1[0x13];
    u32 timer;
    u8 padc8[0x28e];
    u16 delay;
    u32 kind;
    u8 pad35c[0x50];
    u32 flags;
    u8 pad3b0[9];
    u8 alternate;
};
extern u32 gUnk_030053F4;
static inline u8 enterVariant(struct VariantRecoveryActor *p)
{
    if (p->enter) {
        p->enter = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
static inline u8 variantAlternate(struct VariantRecoveryActor *q, u32 offset)
{
    u32 adjusted = offset + 0x61;
    {
        u8 *address = (u8 *)q + adjusted;
        return *address;
    }
}

struct S_08024944 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024944(struct S_08024944 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_080249F0 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 ab;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_080249F0(struct S_080249F0 *p, u32 z)
{
    if (p->b0) {
        p->b0 = z;
        p->af = z;
        return 1;
    }
    return 0;
}

struct S_08024AD4 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024AD4(struct S_08024AD4 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

struct S_08024B60 {
    u8 f0[0x9a];
    u8 a9a;
    u8 f9[0xf];
    u8 aa;
    u8 f1;
    u8 ac;
    u8 ad;
    u8 ae;
    u8 af;
    u8 b0;
    u8 f2[0x13];
    u32 c4;
    u8 f3[0x28e];
    u16 h356;
};

static inline u8 chk_08024B60(struct S_08024B60 *p)
{
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

struct S_08024BEC {
    u8 f0[0xb0];
    u8 b0;
    u8 f1[0x13];
    u32 c4;
};

extern const u32 gUnk_08604E9C[];

extern const u32 gUnk_08604EE0[];

extern u32 gUnk_02000560;

extern const u32 gUnk_08604EF0[];

extern const u32 gUnk_08604EF4[];

extern const u32 gUnk_08604EF8[];

extern u32 gUnk_08604EFC[];

typedef struct ActorAnimation_08024CE0 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation_08024CE0;

static inline void setFacing_08024CE0(ActorAnimation_08024CE0 *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

typedef struct ActorAnimation_08024E18 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation_08024E18;

static inline void setFacing_08024E18(ActorAnimation_08024E18 *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
