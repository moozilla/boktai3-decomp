#include "global.h"
#include "engine.h"
#include "ram.h"
#include "engine_globals.h"
#include "phase_object.h"
#include "damage_number_display.h"
#include "scene_hud_display.h"

s32 Callback_InvokeActor(void *, void *);

static inline u8 ChildGroup_InvokeActorByte(void *actor, void *callback)
{
    return Callback_InvokeActor(actor, callback);
}

struct SwordGroup;
void ChildGroup_LaunchNextInactiveMember(struct SwordGroup *);

/* Twin of ColorPulse_BeginNearApproach: after two approaches, restart the
   timer and hand over to ChildGroup_LaunchNextInactiveMember. */

void ChildGroup_AdvanceLaunchCycle(struct PhaseObject *p)
{
    if (p->phaseUpdate && ChildGroup_InvokeActorByte(p, p->phaseUpdate))
        p->actionCompletions++;
    if (p->actionCompletions > 1) {
        p->actionCompletions = 0;
        p->launchDelay = p->launchInterval;
        p->launchUpdate = (void (*)(struct PhaseObject *))ChildGroup_LaunchNextInactiveMember;
    }
}

struct SwordChild {
    u8 pad0[0x29];
    struct PhaseObjectMode mode;
    u8 pad30[0x74 - 0x30];
    void (*update)(void);
    u8 pad78[0x194 - 0x78];
};

struct SwordGroup {
    u8 unknown0[0x8D];
    struct PhaseObjectMode groupMode;
    u8 unknown94[0x5AC - 0x94];
    struct SwordChild children[3];
    u8 padA68[0xB4C - 0xA68];
    u16 launchDelay, delay;
    u8 unknownB50[0xCC8 - 0xB50];
    void (*update)(struct PhaseObject *);
    u8 unknownCCC[0xCE4 - 0xCCC];
    void (*callback)(struct SwordGroup *);
    u8 unknownCE8[0xD28 - 0xCE8];
    u16 timer;
    u8 unknownD2A[2];
    u8 count;
};

void Sword_BeginRestOrbit(void);

void ChildGroup_WaitForInactiveMembers(struct SwordGroup *);

// confidence=low: launches the next resting sword every launchDelay frames; once all are out,

// waits for them to finish.

static inline void SwordChild_Activate(struct SwordChild *child, u8 active)
{
    child->mode.active = active;
    child->mode.mode = 0;
    child->mode.flags = active | child->mode.flags;
}

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
            if (p->children[i].mode.active == 0) {
                u32 address = i * stride;
                struct SwordChild *child;
                void (**callback)(void);
                u32 offset;
                address += (u32)p;
                address += (u32) & ((struct SwordGroup *)0)->children;
                child = (struct SwordChild *)address;
                {
                    SwordChild_Activate(child, 1);
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

void ChildGroup_BeginInactiveState(struct PhaseObject *);

void ChildGroup_WaitForInactiveMembers(struct SwordGroup *p)
{
    s32 active = 0;
    s32 n;
    u8 *memberActive;
    if (active < p->count) {
        memberActive = (u8 *)p + ((u8 *)&p->children[0].mode.active - (u8 *)p);
        {
            u32 stride = sizeof(struct SwordChild);
            n = p->count;
            do {
                if (*memberActive)
                    active++;
                memberActive += stride;
            } while (--n != 0);
        }
    }
    if (active == 0) {
        PhaseObject_EnterMode((struct PhaseObject *)p, 1, 1);
        p->update = ChildGroup_BeginInactiveState;
    } else if (active < p->count) {
        if (++p->timer <= 7)
            p->callback = ChildGroup_WaitForInactiveMembers;
    }
}

struct DamageChildGroup {
    u8 pad0[0x7E];
    s16 field7E;
    u8 pad80[0xB04];
    struct ContactPosition position;
    u8 unknownB8C[4];
    u8 padB90[0xE0];
    struct DamageNumberSource display;
    u8 unknownSource[0xEC - sizeof(struct DamageNumberSource)];
    s16 fieldD5C;
    u8 fieldD5E;
    u8 padD5F[0x19];
    u16 damage;
    u8 padD7A[2];
    u16 fieldD7C;
    u8 padD7E[8];
    s16 fieldD86;
};

void ChildGroup_ShowDamageNumber(struct DamageChildGroup *p, u32 flags, s32 mode)
{
    struct ContactPosition offset;
    SceneHud_SetTimedState(p->fieldD7C, p->field7E, p->fieldD5C, p->fieldD5E, (flags >> 2) & 1,
                           p->fieldD86);
    ZeroFill8(&offset);
    offset.y = -120;
    DamageNumberDisplay_Show(&p->display, p->damage, mode, &p->position, &offset, 1);
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
    s32 i;
    const u16 *palette = p->sourcePalette;
    for (i = 0; i < 16; i++) {
        p->blended[i] = 0;
        p->source[i] = palette[i];
    }
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

s32 Script_GetValue(void);

static inline s32 ReadParameter(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

void ChildGroup_ReadScriptParameters(struct PhaseObject *p)
{
    if (Script_SeekToKeyword('P')) {
        p->launchInterval = ReadParameter(60);
        p->memberCount = ReadParameter(2);
        p->countdownLimit = ReadParameter(80);
        p->countdownStep = ReadParameter(1);
    } else {
        p->launchInterval = 60;
        p->memberCount = 2;
        p->countdownLimit = 80;
        p->countdownStep = 1;
    }
}

struct ChildGroupEvent {
    u8 pad0[0x40];
    u16 timer;
    u8 pad42[0xE];
    struct PhaseObject *owner;
};

void ChildGroup_UpdateTransitionJitter(struct PhaseObject *);

void ChildGroup_UpdateLandingState(struct PhaseObject *);

s32 ChildGroup_ProcessEvent(struct ChildGroupEvent *event, s32 kind, s32 damage)
{
    struct PhaseObject *p = event->owner;
    if (!p || p->health <= 0)
        return 0;
    switch (kind) {
        case 0:
            event->timer = 0;
            p->hitFrames = 10;
            if (damage) {
                p->damage = damage;
                p->damageKind = 1;
                p->hitResponseD68 = 1;
                p->hitResponseD64 = 0;
                p->hitResponseD60 = 0;
                p->flashTimer = 8;
                AnimData_SelectPaletteBank(p->damageAnimation, p->damagePalette);
            } else {
                p->damage = 0;
                p->damageKind = 0;
                p->hitResponseD68 = 0;
                p->hitResponseD64 = 0;
                p->hitResponseD60 = 0;
                p->update = ChildGroup_UpdateTransitionJitter;
                PhaseObject_EnterMode(p, 1, 7);
            }
            break;
        case 1:
            p->actionCompletions = 0;
            p->update = ChildGroup_UpdateLandingState;
            p->position = p->previousPosition;
            PhaseObject_EnterMode(p, 1, 4);
            break;
    }
    return 1;
}

void ColorPulse_UnpackColor();

void Ivy_UpdateBobbing();

void Ivy_WaitForPrimaryActionCompletion();

void ChildGroup_UpdateTimedOscillation(struct PhaseObject *p)
{
    if (PhaseObject_TakeEntry(p, 1)) {
        p->scatterDelay = 150;
        p->phaseUpdate = ColorPulse_UnpackColor;
    }
    Ivy_UpdateBobbing(p);
    if (--p->scatterDelay == 0) {
        PhaseObject_EnterMode(p, 1, 4);
        p->update = ChildGroup_UpdateLandingState;
        p->transition = Ivy_WaitForPrimaryActionCompletion;
    }
}

void Ivy_InitColorTransition();

static inline u32 ChildGroup_Jitter(u32 value, const s16 *base)
{
    value &= 31;
    return value + (u16)*base - 15;
}

static inline u32 ChildGroup_RandomValue(void)
{
    const u16 *table = gRandomTable;
    u32 *index = &gRandomIndex;
    *index = (*index + 1) & 1023;
    return table[*index];
}

void ChildGroup_UpdateTransitionJitter(struct PhaseObject *p)
{
    s32 i;

    // confidence=low: per-frame update of the child group: resets kid flags, snapshots
    // position, cycles the random table into the position, then runs the callback.
    if (PhaseObject_TakeEntry(p, 1)) {
        p->start.x = p->position.x;
        p->start.y = p->position.y;
        p->start.z = p->position.z;
        p->goal.x = p->target->position.x;
        p->goal.y = 0x15E + p->target->position.y;
        p->goal.z = p->target->position.z;
        p->delta.x = p->goal.x - p->position.x;
        p->delta.y = p->goal.y - p->position.y;
        p->delta.z = p->goal.z - p->position.z;
        for (i = 0; i < 4; i++) {
            p->entries[i].phase = 0x80;
            p->entries[i].flags |= 1;
        }
        p->turnSpeed = 0;
        p->turnFrames = 0;
        p->previousPosition = p->position;
        p->phaseUpdate = Ivy_InitColorTransition;
        p->actionCompletions = 0;
    }
    if (p->hitFrames > 0) {
        p->position.x = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->previousPosition.x);
        p->position.y = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->previousPosition.y);
        p->position.z = ChildGroup_Jitter(ChildGroup_RandomValue(), &p->previousPosition.z);
        p->hitFrames--;
    }
    p->fieldCFC = 0;
    if (p->phaseUpdate != 0) {
        Callback_InvokeActor(p, p->phaseUpdate);
    }
}

void Ivy_WaitForRetarget(struct PhaseObject *);

void ChildGroup_BeginInactiveState(struct PhaseObject *p)
{
    if (PhaseObject_TakeEntry(p, 1)) {
        p->scatterDelay = 100;
        p->retarget = Ivy_WaitForRetarget;
        p->phaseUpdate = ColorPulse_UnpackColor;
    }
    Ivy_UpdateBobbing(p);
    if (p->retarget)
        Callback_InvokeActor(p, p->retarget);
}

void ChildGroup_UpdateTrailingMembers(struct PhaseObject *);

/* After 30 frames, queue the trailing state. */

void ChildGroup_WaitForTrailingMembers(struct PhaseObject *p)
{
    if ((s32)p->frame > 30) {
        p->update = ChildGroup_UpdateTrailingMembers;
        PhaseObject_SetFlags(1, p);
    }
}

void Sound_PlaySong(u32);

void ChildGroup_UpdateTrailingMembers(struct PhaseObject *p)
{
    s32 i;
    if (PhaseObject_TakeEntry(p, 1)) {
        for (i = 0; i < 4; i++) {
            p->entries[i].presentation.x = p->position.x;
            p->entries[i].presentation.y = p->position.y;
            p->entries[i].presentation.z = p->position.z;
            p->entries[i].phase = p->rotation;
        }

        for (i = 0; i < 4; i++)
            p->entries[i].flags &= ~1;

        p->transition = Ivy_WaitForPrimaryActionCompletion;
        p->turnFrames = 0;
        p->turnSpeed = 0;
        p->rotationTimer = 0;
        p->phaseUpdate = Ivy_InitColorTransition;
        p->actionCompletions = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->transition)
        Callback_InvokeActor(p, p->transition);
    if (p->frame & 1) {
        s16 *x0;
        s16 *y0;
        s16 *z0;
        u8 *b0;
        i = 3;
        x0 = &p->entries[0].presentation.x;
        y0 = &p->entries[0].presentation.y;
        z0 = &p->entries[0].presentation.z;
        b0 = &p->entries[0].phase;
        for (; i > 0; i--) {
            p->entries[i].presentation.x = p->entries[i - 1].presentation.x;
            p->entries[i].presentation.y = p->entries[i - 1].presentation.y;
            p->entries[i].presentation.z = p->entries[i - 1].presentation.z;
            p->entries[i].phase = p->entries[i - 1].phase;
        }

        *x0 = p->position.x;
        *y0 = p->position.y;
        *z0 = p->position.z;
        *b0 = p->rotation;
    }
}

void Ivy_WaitForSecondaryActionCompletion(struct PhaseObject *);

s32 Div(s32, s32);

// concession-ok(literal-param): truncating shift helper; the width written in place in its body gives the same code
static inline s32 truncateShift(s32 value, s32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

void ChildGroup_UpdateOrbitingMembers(struct PhaseObject *p)
{
    int i;
    if (PhaseObject_TakeEntry(p, 1)) {
        for (i = 0; i < p->memberCount; i++) {
            p->entries[i].presentation.x = p->position.x;
            p->entries[i].presentation.y = p->position.y;
            p->entries[i].presentation.z = p->position.z;
            p->entries[i].phase = p->rotation;
            p->entries[i].flags &= ~1;
            p->orbitAngles[i] = Div(0x100, p->memberCount) * i;
        }
        p->countdown = 0;
        p->targetAngle = 0;
        p->turnSpeed = 5;
        p->orbitFrames = 0;
        for (i = 0; i < p->memberCount; i++) {
            p->memberCollidersA[i].flags &= ~4;
            p->memberCollidersB[i].flags &= ~4;
        }
        p->secondaryTransition = Ivy_WaitForSecondaryActionCompletion;
        p->phaseUpdate = Ivy_InitColorTransition;
        p->actionCompletions = 0;
        Sound_PlaySong(0x16F);
    }
    Ivy_UpdateBobbing(p);
    i = 0;
    if (i < p->memberCount) {
        s32 *radius = &p->countdown;
        const s16 *cosT = gUnk_086149C4;
        const s16 *sinT = cosT;
        s32 *a = p->orbitAngles;
        do {
            p->entries[i].presentation.x =
                p->position.x + truncateShift(cosT[(*a + 64) & 255] * *radius, 10);
            p->entries[i].presentation.y = p->position.y;
            p->entries[i].presentation.z =
                p->position.z + truncateShift(sinT[*a & 255] * *radius, 10);
            *a += p->turnSpeed;
            if (*a > 255) {
                if (p->orbitFrames > 0x46)
                    Sound_PlaySong(0x169);
                *a -= 255;
            }
            a++;
            i++;
        } while (i < p->memberCount);
    }
    if (p->secondaryTransition)
        Callback_InvokeActor(p, p->secondaryTransition);
    p->orbitFrames++;
}

s32 Terrain_GetHeightAt(void *);

void sub_080F2F24(struct PhaseObject *);

void ChildGroup_UpdateLandingState(struct PhaseObject *p)
{
    if (PhaseObject_TakeEntry(p, 1)) {
        int i;
        p->drawableFlags |= 1;
        p->colliderA.flags |= 4;
        p->colliderB.flags |= 4;
        for (i = 0; i < 4; i++) {
            p->entries[i].presentation.x = p->position.x;
            p->entries[i].presentation.y = p->position.y;
            p->entries[i].presentation.z = p->position.z;
            p->entries[i].phase = p->rotation;
            p->entries[i].flags &= ~1;
            {
                p->memberCollidersA[i].flags |= 4;
                p->memberCollidersB[i].flags |= 4;
            }
            p->landingHeight = (u16)Terrain_GetHeightAt(&p->position);
        }
        p->offsetX = 0;
        p->offsetZ = 0;
        p->scatterStepX = -7;
        p->scatterStepZ = -7;
        p->scatterTransition = sub_080F2F24;
        p->phaseUpdate = Ivy_InitColorTransition;
        p->actionCompletions = 0;
        Sound_PlaySong(0x16F);
    }
    if (p->scatterTransition)
        Callback_InvokeActor(p, p->scatterTransition);
}

void Sword_UpdateSpinningLungeAttack(struct PhaseObject *);

void Sword_UpdateThrustAttack(struct PhaseObject *);

void Sword_UpdateWaveAttack(struct PhaseObject *);

void ChildGroup_UpdateAlternatingCallbacks(struct PhaseObject *p)
{
    if (PhaseObject_TakeEntry(p, 1)) {
        p->launchFrames = 0;
        {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            p->attackHeading = ((const u16 *)RANDOM_TABLE_ADDR)[index];
        }
        p->launchUpdate = ChildGroup_AdvanceLaunchCycle;
        p->attackFrames = 0;
        switch (p->attackMode) {
            case 0:
                p->attackUpdate = Sword_UpdateSpinningLungeAttack;
                break;
            case 1:
                p->attackUpdate = Sword_UpdateThrustAttack;
                break;
        }
        Sound_PlaySong(0x16F);
    }
    Ivy_UpdateBobbing(p);
    if (p->launchUpdate) {
        Callback_InvokeActor(p, p->launchUpdate);
    }
    if ((++p->attackFrames & 0x3FF) == 0) {
        p->attackFrames = 0;
        switch (p->attackMode) {
            case 0:
                p->attackUpdate = Sword_UpdateSpinningLungeAttack;
                break;
            case 1:
                if (p->attackUpdate == Sword_UpdateWaveAttack) {
                    p->attackUpdate = Sword_UpdateThrustAttack;
                } else if (p->attackUpdate == Sword_UpdateThrustAttack) {
                    p->attackUpdate = Sword_UpdateWaveAttack;
                }
                break;
        }
    }
}
