#include "armor_action.h"
#include "global.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"

s32 Armor_TryStartAction(void *, s32);
s32 DirectionalActor_TryBeginState(void *, s32);


void Armor_QueueFallingParts(void *);

void Armor_UpdateAction10Sequence(struct ArmorActionController *p)
{
    u8 phase = p->phase;
    switch (phase) {
        case 0:
            if (Armor_TryStartAction(p, 10) != 0) {
                p->phase = 1;
                p->phaseTimer = phase;
            }
            break;
        case 1:
            if (p->actionState == 0xff) {
                Armor_QueueFallingParts(p->parts);
                p->phase = 2;
                p->phaseTimer = 0;
            }
            break;
        case 2:
            if (p->phaseTimer > 0x13 && Armor_TryStartAction(p, 12) != 0)
                p->phase = 3;
            break;
        case 3:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

#define gRandomTableIndex gRandomIndex

/* Tentative Armor-family behavior: aim a quarter turn to either side of the target. */

struct ArmorActor {
    u8 unk0[0x48];
    s16 x;
    s16 unk4a;
    s16 y;
    u8 unk4e[0xB32 - 0x4E];
    u16 flags;
    u8 unkB34[0xB39 - 0xB34];
    u8 facing;
    u8 unkB3A;
    u8 direction;
    u8 unkB3C;
    u8 phase;
    u8 timer;
    u8 unkB3F;
    u8 actionStatus;
};

struct ArmorTarget {
    u8 unk0[0x30];
    s16 x;
    s16 unk32;
    s16 y;
};

s32 Math_Atan2(s32, s32);


void Armor_UpdateRandomFlankAction42(struct ArmorActor *p)
{
    u8 *q = &p->phase;
    u8 r;
    switch (*q) {
        case 0: {
            struct ArmorTarget *g = gGameState;
            s32 a = Math_Atan2(g->x - p->x, g->y - p->y);
            u32 random;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            random = *(const u16 *)((gRandomIndex * 2) + (u32)gRandomTable);
            random &= 0x10;
            if (random)
                a += 0x40;
            else
                a -= 0x40;
            a += 0x10;
            a &= 0xe0;
            p->direction = a;
            if (DirectionalActor_TryBeginState(p, 0x2a)) {
                p->phase = 1;
            }
            break;
        }
        case 1:
            r = p->actionStatus;
            if (r == 0xff) {
                *q = r;
            }
            break;
    }
}

/* Tentative Armor-family sequence: face target, action 25, delay, action 43. */

u32 ActorController_StartAlternateTurn(struct ArmorActor *, u32);

void Armor_UpdateFacingActionSequence(struct ArmorActor *p)
{
    u8 *q = &p->phase;
    u8 r;
    switch (*q) {
        case 0: {
            struct ArmorTarget *g = gGameState;
            s32 a = Math_Atan2(g->x - p->x, g->y - p->y);
            a = (a + 0x20) & 0xc0;
            if (p->facing != a) {
                if (ActorController_StartAlternateTurn(p, a)) {
                    r = 1;
                    goto st;
                }
                break;
            }
            *q = 1;
        }
        case 1:
            if (DirectionalActor_TryBeginState(p, 0x19))
                p->phase = 2;
            break;
        case 2:
            if (p->timer > 9 && DirectionalActor_TryBeginState(p, 0x2b)) {
                r = 3;
                goto st;
            }
            break;
        case 3:
            r = p->actionStatus;
            if (r == 0xff) {
            st:
                *q = r;
            }
            break;
    }
}

/* tentative: 13-state axis-aligned chase toward a target */

s32 ActorController_StartModeTwoTurn(struct ArmorActor *, s32);

void Armor_UpdateAxisChase(struct ArmorActor *p, s32 fast)
{
    struct ArmorTarget *g = gGameState;
    s32 dx = g->x - p->x;
    s32 dy = g->y - p->y;
    s32 speed = 0x19;
    if (fast)
        speed = 0x2d;
loop:
    switch (p->phase) {
        case 0:
            if (dx > 0) {
                if (p->facing == 0)
                    p->phase = 3;
                else
                    p->phase = 12;
            } else if (dx < 0) {
                if (p->facing == 0x80)
                    p->phase = 1;
                else
                    p->phase = 11;
            } else if (dy >= 0) {
                if (p->facing == 0x40)
                    p->phase = 7;
                else
                    p->phase = 10;
            } else {
                if (p->facing == 0xc0)
                    p->phase = 5;
                else
                    p->phase = 9;
            }
            goto loop;
        case 1:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 2;
            break;
        case 2:
            if (dx > 0x200 || (p->flags & 0x10)) {
                if (dy < 0)
                    p->phase = 9;
                else
                    p->phase = 10;
            }
            break;
        case 3:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 4;
            break;
        case 4:
            if (dx < -0x200 || (p->flags & 0x10)) {
                if (dy < 0)
                    p->phase = 9;
                else
                    p->phase = 10;
            }
            break;
        case 5:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 6;
            break;
        case 6:
            if (dy > 0x200 || (p->flags & 0x10)) {
                if (dx < 0)
                    p->phase = 11;
                else
                    p->phase = 12;
            }
            break;
        case 7:
            if (DirectionalActor_TryBeginState(p, speed))
                p->phase = 8;
            break;
        case 8:
            if (dy < -0x200 || (p->flags & 0x10)) {
                if (dx < 0)
                    p->phase = 11;
                else
                    p->phase = 12;
            }
            break;
        case 9:
            if (ActorController_StartModeTwoTurn(p, 0xc0))
                p->phase = 5;
            break;
        case 10:
            if (ActorController_StartModeTwoTurn(p, 0x40))
                p->phase = 7;
            break;
        case 11:
            if (ActorController_StartModeTwoTurn(p, 0x80))
                p->phase = 1;
            break;
        case 12:
            if (ActorController_StartModeTwoTurn(p, 0))
                p->phase = 3;
            break;
    }
}


void Armor_UpdatePlayerAlignmentAction25(u8 *p)
{
    Armor_UpdateAxisChase(p, 0);
}


void Armor_UpdatePlayerAlignmentAction45(s32 a)
{
    Armor_UpdateAxisChase(a, 1);
}

struct Armor {
    u8 unknown[0xB38];
    u8 variant;
    u8 unknownB39[4];
    u8 sequenceState;
    u8 sequenceTimer;
    u8 unknownB3F;
    u8 actionState;
};


void Armor_StartSlowWhiteFade(struct Armor *);

void SceneCombatActor_ResetAttackMode(struct Armor *);

void SceneCombatActor_ResetDefenseMode(struct Armor *);

void Armor_BindPaletteBanks(struct Armor *);

void Armor_UpdateWhiteFadeSequence(struct Armor *armor)
{
    switch (armor->sequenceState) {
        case 0:
            if (Armor_TryStartAction(armor, 0x31))
                armor->sequenceState = 1;
            break;
        case 1:
            if (armor->actionState == 0xFF) {
                Armor_StartSlowWhiteFade(armor);
                armor->sequenceState = 2;
                armor->sequenceTimer = 0;
            }
            break;
        case 2:
            if (armor->sequenceTimer > 15) {
                switch (armor->variant) {
                    case 12:
                    case 13:
                        SceneCombatActor_ResetAttackMode(armor);
                        break;
                    case 14:
                    case 15:
                        SceneCombatActor_ResetDefenseMode(armor);
                        break;
                }
                Armor_BindPaletteBanks(armor);
                armor->sequenceState = 3;
            }
            break;
        case 3:
            if (Armor_TryStartAction(armor, 0x32)) {
                armor->sequenceState = 4;
                armor->sequenceTimer = 0;
            }
            break;
        case 4:
            if (armor->actionState == 0xFF)
                armor->sequenceState = 0xFF;
            break;
    }
}

struct S {
    u8 f0[0x48];
    s16 a48;
    s16 a4a;
    s16 a4c;
    u8 g0[0xB3A - 0x4E];
    u8 b3a;
    u8 f1;
    u8 b3c;
    u8 b3d;
    u8 b3e;
    u8 b3f;
    u8 b40;
    u8 f2[4];
    u8 b45;
    u8 g1[0x1124 - 0xB46];
    s16 x;
    s16 y;
    s16 z;
};

void ActorController_StartTurn(struct S *, s32);


void Armor_UpdateTurnTriggerCompletion(struct S *p)
{
    switch (p->b3d) {
        case 0:
            ActorController_StartTurn(p, p->b3a);
            p->b3d++;
            break;
        case 1:
            if (p->b40 == 0xff) {
                p->b3d = 0xff;
                Trigger_ConsumePending((u8 *)p + 0xB40 - 0x64, 1);
            }
            break;
    }
}

struct DirectionalActorDisplay {
    u32 unknown0[2], flags;
};

struct DirectionalActor {
    u8 unknown0[0xB4];
    struct DirectionalActorDisplay display;
    u8 unknownC0[0xA1C];
    u8 trigger[0x56];
    u16 flagsB32;
    u8 unknownB34[9];
    u8 pendingVisibility, delay;
};


void DirectionalActor_UpdateVisibilityDelay(struct DirectionalActor *p)
{
    s32 mode = p->pendingVisibility;
    switch (mode) {
        case 0:
        case 1:
            if (p->delay) {
                p->delay--;
            } else {
                if (mode != 0)
                    ((struct DirectionalActorDisplay *)((u8 *)p + 0xB4))->flags |= 1;
                else
                    ((struct DirectionalActorDisplay *)((u8 *)p + 0xB4))->flags &= ~1;
                p->flagsB32 &= 0xfbff;
                p->pendingVisibility = 0xff;
                Trigger_ConsumePending(p->trigger, 1);
            }
            break;
    }
}


void Armor_StartAction47(s32 a)
{
    DirectionalActor_TryBeginState(a, 0x2f);
}

void Armor_UpdateAction48Completion(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (Armor_TryStartAction(p, 0x30) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

extern const u32 gUnk_0824F014[];


void Armor_DispatchBehaviorUpdate(struct S *p)
{
    const u32 *t = gUnk_0824F014;
    u8 *q = &p->b3c;
    Callback_InvokeActor(p, t[*q]);
    switch (*q) {
        case 0x20:
        case 0x1b:
        case 0x1c:
        case 0x22:
            break;
        default:
            p->b45 = 0;
            break;
    }
    p->b3e++;
}

s32 Armor_IsNearSavedPosition(struct S *p)
{
    s32 dx = p->x - p->a48;
    s32 dy = p->z - p->a4c;
    s32 r;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    r = 0;
    if (dx <= 0xbf && dy <= 0xbf)
        r = 1;
    return r;
}

struct P {
    u8 f0[0x48];
    u16 a;
    u16 b;
    u16 c;
    u8 f1[0x1124 - 0x4E];
    u16 x;
    u16 y;
    u16 z;
};

void Armor_SaveCurrentPosition(struct P *p)
{
    p->x = p->a;
    p->y = p->b;
    p->z = p->c;
}
