#include "global.h"
#include "engine/engine.h"
#include "script/script_utility.h"
#include "engine/math/scene_vec.h"
#include "scene/collision/contact_collider.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"

struct IvyBurstVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

struct IvyBurstScene {
    u8 unknown0[0x1C];
    u32 mode;
};

struct SwordPhase {
    u8 flags;
    u8 unknown1[4];
    u8 state, phase;
} __attribute__((packed));

struct SwordMotion {
    u8 unknown0[0x29];
    struct SwordPhase phase;
    u8 unknown30[0x44];
    void *update;
    u8 unknown78[0x2C];
};

struct SwordComponent {
    u8 unknown0[0xF0];
    struct SwordMotion motion;
};

struct IvyBurstPart {
    u32 flags;
    u8 unknown4[0x28];
};

struct IvyBurstPrefix {
    u8 unknown0[0x60];
    struct IvyBurstScene *scene;
    u8 unknown64[0x22];
    s16 lifeParameter;
    u8 unknown88[5];
    struct SwordPhase control;
    u8 unknown94[0xC];
    u8 flash, bursts, started, unknownA3;
    u16 lifeValue;
    u8 unknownA6[2];
    struct IvyBurstPart parts[4];
    u8 unknown158[0x70];
    struct ContactCollider attack[4];
    struct ContactCollider body[4];
    u8 unknown468[0x54];
    struct SwordComponent children[4];
    u8 unknownB0C[0x5C];
    u32 effectFlags;
    u8 unknownB6C[2];
    u8 direction;
    u8 unknownB6F[0x15];
    struct IvyBurstVector position;
    u8 unknownB8C[0x24];
    u8 shadow[0x6C];
    struct ContactCollider colliderA, colliderB;
    u8 unknownCC4[0x2C];
    void *callback;
    u8 unknownCF4[0x3A];
    u16 counter;
    u8 unknownD30[0x24];
    u8 done, bestiaryIndex;
    u8 unknownD56[9];
    u8 burstIndex;
    void *lifeState;
    u8 unknownD64[8];
    u32 script;
    s32 scriptArgument0, scriptArgument1;
};

extern const s16 gUnk_086149C4[];

void TargetMotion_UpdateFlashingExit(void);

void Player_AddExperience(u32);

void Player_RefreshLifeState(void *, s32);

void ScaledShadow_Disable(void *);

s32 SparkParticle_Spawn(const void *, u8, const void *, s32, s32, s32, s32, u32);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

s32 Mod(s32, s32);

static inline void Ivy_ClearStart(struct SwordPhase *control, u32 bits)
{
    u32 mask = ~bits;
    control->flags = mask & control->flags;
}

static inline void Ivy_SetFlags(struct SwordPhase *control, u8 bits)
{
    control->flags = bits | control->flags;
}

static inline u8 Ivy_TakeBurstStart(struct IvyBurstPrefix *actor)
{
    u8 mask = 1;
    if (mask & actor->control.flags) {
        Ivy_ClearStart(&actor->control, 1);
        actor->started = 0;
        return 1;
    }
    return 0;
}

static inline s32 Ivy_ScaleOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void Sword_SetPhase(struct SwordPhase *phase, u8 active, u8 state)
{
    phase->state = active;
    phase->phase = state;
    phase->flags |= active;
}

static inline u8 Sword_ChildIsActive(struct SwordComponent *child)
{
    return child->motion.phase.state;
}

static inline struct SwordPhase *Sword_ChildPhase(struct IvyBurstPrefix *actor, s32 index)
{
    return &actor->children[index].motion.phase;
}

static inline struct SwordMotion *Sword_ChildMotion(struct IvyBurstPrefix *actor, s32 index)
{
    return &actor->children[index].motion;
}

void Ivy_UpdateDefeatBurst(struct IvyBurstPrefix *actor)
{
    s32 remainder;
    if (Ivy_TakeBurstStart(actor)) {
        s32 i = 0;
        struct IvyBurstPart *parts = actor->parts;
        for (; i <= 3; i++) {
            if (Sword_ChildIsActive(&actor->children[i])) {
                struct SwordMotion *motion = Sword_ChildMotion(actor, i);
                Sword_SetPhase(Sword_ChildPhase(actor, i), 1, 5);
                motion->update = TargetMotion_UpdateFlashingExit;
            }
        }

        for (i = 0; i <= 3; i++) {
            u32 *flags;
            flags = &parts[i].flags;
            if (!((*flags) & 1)) {
                *flags |= 1;
                actor->attack[i].flags |= 4;
                actor->body[i].flags |= 4;
            }
        }

        actor->colliderA.flags |= 4;
        actor->colliderB.flags |= 4;
        actor->counter = 0;
        actor->burstIndex = 0;
        actor->flash = 0;
        actor->bursts = 30;
        Player_AddExperience(actor->lifeValue);
        Player_RefreshLifeState(actor->lifeState, actor->lifeParameter);
        actor->done = 1;
    }
    if (actor->callback)
        Callback_InvokeActor(actor, actor->callback);
    remainder = Mod(actor->counter, 2);
    if (remainder == 0) {
        struct IvyBurstVector range;
        struct IvyBurstVector position;
        struct SceneVec offset;
        actor->effectFlags |= 1;
        actor->flash = 0;
        actor->bursts--;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position = actor->position;
        ZeroFill8(&offset);
        switch (actor->burstIndex) {
            case 0:
                offset.z = (offset.x = -120);
                actor->burstIndex++;
                Sound_PlaySong(366);
                break;

            case 1:
                offset.z = (offset.x = -75);
                actor->burstIndex++;
                break;

            case 2:
                offset.z = (offset.x = -30);
                actor->burstIndex++;
                break;

            case 3:
                offset.z = (offset.x = 15);
                actor->burstIndex++;
                break;

            case 4:
                offset.z = (offset.x = 60);
                actor->burstIndex = 0;
                break;
        }

        position.x = (Ivy_ScaleOffset(offset.x * gUnk_086149C4[(actor->direction + 64) & 255]) +
                      Ivy_ScaleOffset(offset.z * gUnk_086149C4[actor->direction])) +
                     position.x;
        position.z = (Ivy_ScaleOffset(-(offset.x * gUnk_086149C4[actor->direction])) +
                      Ivy_ScaleOffset(offset.z * gUnk_086149C4[(actor->direction + 64) & 255])) +
                     position.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
        if (!actor->bursts) {
            ScaledShadow_Disable(actor->shadow);
            actor->control.state = 0;
            actor->control.phase = 0;
            Ivy_SetFlags(&actor->control, 1);
            if ((actor->scene->mode != 4) && (actor->script)) {
                s32 values[8];
                struct ScriptUtilityArguments arguments;
                values[0] = actor->position.x;
                values[1] = actor->position.y;
                values[2] = actor->position.z;
                values[3] = actor->scriptArgument0;
                values[4] = actor->scriptArgument1;
                values[5] = -1;
                values[6] = -1;
                values[7] = -1;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(actor->script, &arguments);
                Bestiary_UnlockEntry(Bestiary_GetEntryForActor(11, actor->bestiaryIndex));
            }
        }
    } else {
        actor->effectFlags &= ~1;
        actor->flash++;
    }
    (actor->counter)++;
}

/* Ivy_OnUpdateIdle: Empty callback at the end of the main Ivy state handlers; idle role tentative */

void Ivy_OnUpdateIdle(void)
{
}

struct P {
    u8 f[0xcc8];
    void *a;
    u8 g[0xcd0 - 0xccc];
    void *b;
};

s32 Callback_InvokeActor(void *, void *);

void Ivy_UpdateGroundHeight(void *);

void Ivy_UpdateActions(struct P *p)
{
    if (p->a != 0) {
        Callback_InvokeActor(p, p->a);
        if (p->b != 0)
            Callback_InvokeActor(p, p->b);
        Ivy_UpdateGroundHeight(p);
    }
}

extern void (*gUnk_08606434[])(void);

void Ivy_DispatchState(struct IvyBurstPrefix *actor)
{
    u32 m = 0x10;
    if ((actor->control.flags & m) == 0)
        Callback_InvokeActor(actor, gUnk_08606434[actor->control.state]);
}

void Ivy_OnChildDestroy(void)
{
}

struct SwordContact {
    u8 pad0[0x6];
    u16 flags;
    u8 pad8[0x4];
    u16 pos[3];
    u8 pad12[0x22 - 0x12];
    u16 force;
    u8 pad24[0x34 - 0x24];
    u32 contactFlags;
    u8 pad38[0x3e - 0x38];
    u16 damage;
    u8 pad40[0x42 - 0x40];
    u8 direction;
    u8 pad43[0x44 - 0x43];
    u16 responseValue;
};

struct SwordContactOwner {
    u8 unknown0[0xD1A];
    s16 contactHundredths;
    u8 unknownD1C[0x3C];
    u16 palette;
};

struct SwordComponentOnChildContact {
    u8 pad0[0xc];
    u8 *animation;
    u8 pad10[0x52 - 0x10];
    u16 field52;
    u8 pad54[0x6a - 0x54];
    u16 force;
    u8 pad6c[0xa6 - 0x6c];
    u16 fieldA6;
    u8 padA8[0xe0 - 0xa8];
    u16 disabled;
    u8 padE2[0x100 - 0xe2];
    s16 velocityX;
    u8 pad102[2];
    s16 velocityZ;
    u8 pad106[0x10a - 0x106];
    s16 health;
    u8 pad10c[0x119 - 0x10c];
    u8 flags;
    u8 direction;
    u8 pad11b[0x11e - 0x11b];
    u8 active;
    u8 phase;
    u8 pad120[0x128 - 0x120];
    u16 paletteTimer;
    u8 pad12a[0x160 - 0x12a];
    struct SwordContactOwner *owner;
    void (*update)(void);
    void (*paletteUpdate)(void);
    u8 pad16c[0x174 - 0x16c];
    void (*returnUpdate)(void);
    void (*thrustUpdate)(void);
    void (*lungeUpdate)(void);
    u8 pad180[0x184 - 0x180];
    u32 timer;
    u8 unknown188[0xC];
};

void Collision_CalculateDamageResponse(void *, void *);

void PlayerStats_AddContactHundredths(void *, void *, s32);

void Sword_SetTrajectoryFromCurrentPosition(void *);

void Sword_UpdateComponentPaletteRestore(void);

void Sword_DecelerateReturn(void);

void Sword_UpdateThrustDelay(void);

void Sword_UpdateThrustExtension(void);

void Sword_UpdateLungeSpinup(void);

void Sword_UpdateSpinningLunge(void);

void Sword_DampTargetReturn(void);

void ActorPhase_DampMovement(void);

void TargetMotion_AdvanceFollowSegment(void);

static inline u32 Sword_ContactFlags(struct SwordContact *contact, u32 mask)
{
    return contact->contactFlags & mask;
}

static inline void Sword_SetChildState(struct SwordComponentOnChildContact *child, u8 active,
                                       u8 phase)
{
    child->active = active;
    child->phase = phase;
    child->flags |= active;
}

static inline void Sword_ContactDisplacement(struct SceneVec *out, const struct SceneVec *target,
                                             const struct SceneVec *source)
{
    out->x = source->x - target->x;
    out->y = source->y - target->y;
    out->z = source->z - target->z;
}

static inline u32 Sword_GetPhase(struct SwordComponentOnChildContact *child, u8 **field)
{
    u32 value = child->phase;
    *field = &child->phase;
    return value;
}

static inline void Sword_BeginContactReturn(struct SwordComponentOnChildContact *child,
                                            u8 *phaseField)
{
    u32 phase = *phaseField;
    if (phase == 2) {
        child->timer = 0x80;
        child->returnUpdate = Sword_DecelerateReturn;
    } else if (phase == 3) {
        child->timer = 0x14;
        child->thrustUpdate = ActorPhase_DampMovement;
    } else if (phase == 4) {
        child->timer = 0x64;
        child->lungeUpdate = Sword_DampTargetReturn;
    }
}

void Ivy_OnChildContact(struct SwordContact *source, struct SwordContact *target,
                        struct SwordComponentOnChildContact *child)
{
    struct SceneVec displacement;
    s16 *health;
    struct SwordContactOwner *owner;
    u32 phaseSnapshot;
    u8 *phase;

    owner = child->owner;
    health = &child->health;
    if (*health <= 0)
        return;
    if (Sword_ContactFlags(source, 0x2000))
        return;
    if (child->disabled)
        return;
    Collision_CalculateDamageResponse(source, target);
    target->responseValue = source->responseValue;
    if (child->paletteUpdate == 0 && target->damage != 0) {
        child->paletteTimer = 8;
        child->paletteUpdate = Sword_UpdateComponentPaletteRestore;
        AnimData_SelectPaletteBank(child->animation, owner->palette);
        *health -= target->damage;
        PlayerStats_AddContactHundredths(source, target, owner->contactHundredths);
        if (*health <= 0) {
            Sword_SetChildState(child, 1, 5);
            child->update = TargetMotion_UpdateFlashingExit;
            Sound_PlaySong(0x13E);
            return;
        }
    }

    phaseSnapshot = Sword_GetPhase(child, &phase);
    if ((phaseSnapshot == 2 && child->returnUpdate != Sword_DecelerateReturn) ||
        (phaseSnapshot == 3 && (child->thrustUpdate == Sword_UpdateThrustDelay ||
                                child->thrustUpdate == Sword_UpdateThrustExtension)) ||
        (phaseSnapshot == 4 && (child->lungeUpdate == Sword_UpdateLungeSpinup ||
                                child->lungeUpdate == Sword_UpdateSpinningLunge ||
                                child->lungeUpdate == TargetMotion_AdvanceFollowSegment))) {
        Sword_SetTrajectoryFromCurrentPosition(child);
        Sword_BeginContactReturn(child, phase);

        if (source->flags & 0x100) {
            child->direction = source->direction;
        } else {
            Sword_ContactDisplacement(&displacement, (void *)target->pos, (void *)source->pos);
            if (displacement.x == 0 && displacement.z == 0) {
                u32 *counter = &gRandomIndex;
                *counter = (*counter + 1) & 0x3FF;
                child->direction = ((const u16 *)RANDOM_TABLE_ADDR)[*counter];
            } else {
                child->direction = Math_Atan2(displacement.x, displacement.z);
            }
        }
        child->fieldA6 = 1;
        child->field52 = 1;
        child->force = source->force;
        child->velocityX = gUnk_086149C4[(child->direction + 0x40) & 0xFF] >> 7;
        child->velocityZ = gUnk_086149C4[child->direction] >> 7;
        Sound_PlaySong(0x170);
    } else {
        if (target->damage != 0)
            Sound_PlaySong(0x13E);
        else
            Sound_PlaySong(0x190);
    }
}

struct S {
    u8 filler[0x1e];
    u16 h1e;
    u8 f2[0xf2 - 0x20];
    u16 hf2;
    u8 f3[0xfa - 0xf4];
    u16 hfa;
    u8 f4[0x180 - 0xfc];
    u32 cnt;
};

void Ivy_UpdateChildBobbing(struct S *s)
{
    u32 *c = &s->cnt;
    if ((*c & 3) == 0)
        s->hf2 += s->hfa;
    *c = *c - 1;
    if (*c == 0) {
        s->hfa = -s->hfa;
        *c = 0x28;
    }
    s->h1e += s->hf2;
}

struct A {
    u8 p0[6];
    u8 b6, b7, b8, b9;
    u8 pA[0x16c - 0xa];
    void (*fn)(void);
    u8 p170[0x18e - 0x170];
    u8 b18e, b18f;
};

void Sound_PlaySong(u32 song); /* m4aSongNumStart, remembering the song per track unless muted */

struct Obj;

void Ivy_ApproachChildBlend(struct Obj *p);

void Ivy_BeginChildPulse(struct A *p)
{
    p->b6 = 0x80;
    p->b8 = 0x7f;
    p->b9 = 1;
    p->b18e = 0xf8;
    p->b18f = 0x10;
    p->fn = Ivy_ApproachChildBlend;
    Sound_PlaySong(0x16d);
}

struct Obj {
    u32 flags;
    u8 pad4[0x2];
    u8 rotation;
    u8 pad7[0x1];
    s8 byte8;
    s8 byte9;
    u8 padA[0x12];
    s16 x;
    s16 y;
    s16 z;
    u8 pad22[0x2c];
    u16 flags4E;
    u8 pad50[0x52];
    u16 flagsA2;
    u8 padA4[0x4e];
    u16 velocity;
    u8 padF4[0x40];
    u16 endX;
    s16 endY;
    u16 endZ;
    u8 pad13A[0x2];
    u16 startX;
    u16 startY;
    u16 startZ;
    u8 pad142[0x2];
    s16 dx;
    s16 dy;
    s16 dz;
    u8 pad14A[0x12];
    u8 *parent;
    u8 pad160[0xc];
    void (*callback)(struct Obj *);
    u8 pad170[0x1e];
    s8 delta8;
    s8 delta9;
};

struct PhaseActor;

void ActorPhase_ApproachNeutral(struct PhaseActor *p);

void Ivy_ApproachChildBlend(struct Obj *p)
{
    s32 a = p->byte8, b = p->byte9;
    if (a > 64) {
        a += p->delta8;
        if (a < 64)
            a = 64;
    }
    if (b < 127) {
        b += p->delta9;
        if (b > 127)
            b = 127;
    }
    p->byte8 = a;
    p->byte9 = b;
    if (b == 127) {
        p->byte8 = 64;
        p->delta9 = -16;
        p->callback = ActorPhase_ApproachNeutral;
    }
}

struct PhaseActor {
    u8 unknown0[9];
    s8 phase;
    u8 unknownA[0x10F];
    u8 flags119;
    u8 unknown11A[4];
    u8 field11E, field11F;
    u8 unknown120[0x44];
    void (*update164)(void);
    u8 unknown168[4];
    void (*update16C)(void);
    u8 unknown170[0x1A];
    s8 direction;
    u8 unknown18B[4];
    u8 phaseStep;
};

void Ivy_BeginChildPulse(struct A *p);

void IvyChild_UpdateAfterBob(void);

void ActorPhase_ApproachNeutral(struct PhaseActor *p)
{
    s32 phase = p->phase;
    if (phase > 64) {
        phase += (s8)p->phaseStep;
        if (phase < 64)
            phase = 64;
    }
    p->phase = phase;
    if (phase == 64) {
        u32 random;
        u8 one;
        u32 offset;
        p->update16C = Ivy_BeginChildPulse;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        random = *(const u16 *)(offset + (u32)gRandomTable);
        one = 1;
        random &= one;
        p->direction = random ? 1 : -1;
        p->field11E = one;
        p->field11F = one;
        p->flags119 = one | p->flags119;
        p->update164 = IvyChild_UpdateAfterBob;
    }
}

struct PBeginChildRandomDelay {
    u8 f[0x170];
    void (*cb)(void);
    u8 g[0x184 - 0x174];
    u32 a;
};

void ActorPhase_OrbitParent(void);

void Ivy_BeginChildRandomDelay(struct PBeginChildRandomDelay *p)
{
    u32 v;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    v = *(u16 *)(((u8 *)gRandomTable) + gRandomIndex * 2);
    p->a = (v & 0x40) + 0x1e;
    p->cb = ActorPhase_OrbitParent;
}
