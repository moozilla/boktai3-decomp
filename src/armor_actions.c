#include "global.h"
#include "armor_action.h"
#include "player.h"
#include "ram.h"
#include "engine_globals.h"

extern u8 *gGameState;

struct ArmorFacingAction6Actor {
    u8 f0[0x48];
    s16 a48;
    s16 a4a;
    s16 a4c;
    u8 g0[0xB39 - 0x4E];
    u8 b39;
    u8 b3a;
    u8 f1;
    u8 b3c;
    u8 b3d;
    u8 b3e;
    u8 b3f;
    u8 b40;
    u8 b41;
    u8 b42;
    u8 b43;
    u8 f2;
    u8 b45;
    u8 g1[0x1124 - 0xB46];
    s16 x;
    s16 y;
    s16 z;
};

struct ArmorPlayerPosition {
    u8 f[0x30];
    s16 x;
    s16 pad;
    s16 y;
};

s32 Math_Atan2(s32, s32);



void Armor_UpdateFacingAction6(struct ArmorFacingAction6Actor *p)
{
    u8 *q = &p->b3d;
    u8 r;
    switch (*q) {
        case 0: {
            struct ArmorPlayerPosition *g = gGameState;
            s32 a = Math_Atan2(g->x - p->a48, g->y - p->a4c);
            a = (a + 0x20) & 0xc0;
            if (p->b39 != a)
                ActorController_StartTurn(p, a);
            else if (Armor_TryStartAction(p, 6)) {
                r = 1;
                goto st;
            }
            break;
        }
        case 1:
            r = p->b40;
            if (r == 0xff) {
            st:
                *q = r;
            }
            break;
    }
}

struct P {
    u32 x : 16, y : 16, z : 16, w : 16;
};



void Armor_UpdateFacingAction9(u8 *p)
{
    u8 *state = p + 0xb3d;
    switch (*state) {
        case 0: {
            struct P v;
            u8 *actor = gGameState;
            s32 angle;
            v.x = *(u16 *)(actor + 0x30);
            v.y = *(u16 *)(actor + 0x32);
            v.z = *(u16 *)(actor + 0x34);
            angle = Math_Atan2((s16)v.x - *(s16 *)(p + 0x48), (s16)v.z - *(s16 *)(p + 0x4c));
            angle = (angle + 0x20) & 0xc0;
            if (p[0xb39] != angle)
                ActorController_StartTurn(p);
            else if (Armor_TryStartAction(p, 9))
                *state = 1;
            break;
        }
        case 1:
            if (p[0xb40] == 0xff)
                *state = 0xff;
            break;
    }
}

void Armor_UpdateFacingAction29(u8 *p)
{
    u8 *state = p + 0xb3d;
    switch (*state) {
        case 0: {
            struct P v;
            u8 *actor = gGameState;
            s32 angle;
            v.x = *(u16 *)(actor + 0x30);
            v.y = *(u16 *)(actor + 0x32);
            v.z = *(u16 *)(actor + 0x34);
            angle = Math_Atan2((s16)v.x - *(s16 *)(p + 0x48), (s16)v.z - *(s16 *)(p + 0x4c));
            angle = (angle + 0x20) & 0xc0;
            if (p[0xb39] != angle)
                ActorController_StartTurn(p);
            else if (Armor_TryStartAction(p, 29))
                *state = 1;
            break;
        }
        case 1:
            if (p[0xb40] == 0xff)
                *state = 0xff;
            break;
    }
}

struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};


void Armor_StartAction7(struct E *p)
{
    if (p->a == 0) {
        if (Armor_TryStartAction(p, 7) != 0)
            p->a = 0xff;
    }
}

void Armor_UpdateAction8(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (Armor_TryStartAction(p, 8) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

struct ArmorProjectileActor {
    u8 pad0[0xd4];
    u16 position[3];
    u8 padDA[0xb26 - 0xda];
    s16 projectileParam;
    u8 padB28[0xb39 - 0xb28];
    u8 direction;
};

struct ArmorProjectileActorProjectileOffset {
    u16 x, y, z, pad;
};

extern const struct ArmorProjectileActorProjectileOffset gUnk_0824EFDC[];

void ActorEffect_Spawn(u16 *, void *, s32, s32, s32, s32, s32, s32, s32, s32);

void Armor_SpawnPairedProjectiles(struct ArmorProjectileActor *p)
{
    u8 *owner = (*(u8 **)gPlayers);
    s32 direction = p->direction >> 6;
    s32 i = 0;
    u16 pos[3];
    u16 *out = pos;
    u16 *base = p->position;
    s32 offsetBytes = direction << 3;
    for (; i < 2; i++) {
        const struct ArmorProjectileActorProjectileOffset *offset =
            (const struct ArmorProjectileActorProjectileOffset *)((const u8 *)gUnk_0824EFDC + offsetBytes);
        out[0] = base[0] + offset->x;
        out[1] = base[1] + offset->y;
        out[2] = base[2] + offset->z;
        ActorEffect_Spawn(pos, owner + 0x30, p->projectileParam, 20, 0x40000, 30, 20, 120, 240,
                          ((i << 6) + p->direction - 32) & 255);
        offsetBytes += 8;
    }
}

struct S {
    u8 f0[0x48];
    s16 a48;
    s16 a4a;
    s16 a4c;
    u8 g0[0xB39 - 0x4E];
    u8 b39;
    u8 b3a;
    u8 f1;
    u8 b3c;
    u8 b3d;
    u8 b3e;
    u8 b3f;
    u8 b40;
    u8 b41;
    u8 b42;
    u8 b43;
    u8 f2;
    u8 b45;
    u8 g1[0x1124 - 0xB46];
    s16 x;
    s16 y;
    s16 z;
};





void Armor_UpdatePairedProjectileAttack(struct S *p)
{
    u8 s = p->b3d;
    switch (s) {
        case 0:
            if (Armor_TryStartAction(p, 0xa)) {
                p->b3d = 1;
                p->b3e = s;
            }
            break;
        case 1:
            if (DirectionalActor_TryBeginState(p, 0xc)) {
                p->b3d = 2;
                p->b3e = 0;
                Armor_SpawnPairedProjectiles(p);
            }
            break;
        case 2:
            if (p->b40 == 0xff)
                p->b3d = 0xff;
            break;
    }
}

struct ArmorScriptedActor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad4E[0xb3d - 0x4e];
    u8 state, timer;
    u8 padB3F;
    u8 animationState;
    u8 padB41[0xb54 - 0xb41];
    u32 attackScript;
};

struct ArmorScriptedActorScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};



void Script_Call(u32, void *);

void Armor_UpdateScriptedAttack(struct ArmorScriptedActor *p)
{
    s32 values[3];
    struct ArmorScriptedActorScriptArgs args;
    u8 state = p->state;
    switch (state) {
        case 0:
            if (Armor_TryStartAction(p, 10)) {
                p->state = 1;
                p->timer = state;
            }
            break;
        case 1:
            if (DirectionalActor_TryBeginState(p, 12)) {
                u32 script;
                p->state = 2;
                p->timer = 0;
                script = p->attackScript;
                if (script) {
                    args.count = 3;
                    args.values = values;
                    values[0] = p->x;
                    values[1] = p->y;
                    values[2] = p->z;
                    Script_Call(script, &args);
                }
            }
            break;
        case 2:
            if (p->animationState == 255)
                p->state = 255;
            break;
    }
}

void SpriteColliderGroup_ActivateParts(void *, s32);

void Armor_UpdateDelayedAction12(struct S *p)
{
    u8 s = p->b3d;
    switch (s) {
        case 0:
            if (Armor_TryStartAction(p, 0xa)) {
                p->b3d = 1;
                p->b3e = s;
            }
            break;
        case 1:
            if (p->b40 == 0xff) {
                SpriteColliderGroup_ActivateParts((u8 *)p + 0xB58, 0x64);
                if (p->b3e > 0x13) {
                    if (Armor_TryStartAction(p, 0xc))
                        p->b3d = 3;
                }
            }
            break;
        case 3:
            if (p->b40 == 0xff)
                p->b3d = 0xff;
            break;
    }
}

struct ArmorAction13Actor {
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



void Armor_UpdateAction13Transition(struct ArmorAction13Actor *p)
{
    switch (p->b3d) {
        case 0:
            if (DirectionalActor_TryBeginState(p, 0xd))
                p->b3d = 1;
            break;
        case 1:
            if (p->b40 == 0xff)
                ActorController_SetBehavior(p, 0xb);
            break;
    }
}


void Armor_StartAction14(struct E *p)
{
    if (p->a == 0) {
        if (DirectionalActor_TryBeginState(p, 0xe) != 0)
            p->a = 0xff;
    }
}

struct ArmorTargetFacingActor {
    u8 pad0[0x18];
    u8 navigation[0x30];
    s16 position[3];
    u8 pad4E[0xb39 - 0x4e];
    u8 direction;
    u8 padB3A[3];
    u8 state, timer;
};

struct ArmorTargetFacingActorPackedPosition {
    u32 x : 16, y : 16, z : 16, unused : 16;
};

s32 Terrain_ApplyBlocked(void *, void *, struct ArmorTargetFacingActorPackedPosition *);

u32 sub_0821FB8C(void *, void *, u32, u32);


void Armor_UpdateTargetFacing(struct ArmorTargetFacingActor *p)
{
    struct ArmorTargetFacingActorPackedPosition pos;
    if ((p->timer & 3) == 0) {
        u8 *target = gGameState;
        pos.x = *(u16 *)(target + 0x30);
        pos.y = *(u16 *)(target + 0x32);
        pos.z = *(u16 *)(target + 0x34);
        Terrain_ApplyBlocked(p->navigation, p->position, &pos);
    }
    sub_0821FB8C(p->navigation, p->position, p->direction, 0x40);
    if (p->state == 0) {
        if (DirectionalActor_TryBeginState(p, 0x1c))
            p->state = 255;
    }
}


void Armor_UpdateAction15(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (DirectionalActor_TryBeginState(p, 0xf) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

struct ArmorAction16Actor {
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



void Armor_UpdateAction16Transition(struct ArmorAction16Actor *p)
{
    switch (p->b3d) {
        case 0:
            ActorController_ForceAction(p, 0x10);
            p->b3d = 1;
            break;
        case 1:
            if (p->b40 == 0xff)
                ActorController_SetBehavior(p, 0xb);
            break;
    }
}


void Armor_UpdateAction17(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            ActorController_ForceAction(p, 0x11);
            p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

void Armor_UpdateAction4(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            ActorController_ForceAction(p, 4);
            p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}

void Armor_UpdateAction5(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            ActorController_ForceAction(p, 5);
            p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}


void Armor_StartAction4(struct E *p)
{
    if (p->a == 0) {
        ActorController_ForceAction(p, 4);
        p->a = 1;
    }
}

struct ArmorDefeatActor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad4E[0xb18 - 0x4e];
    u32 scriptValue;
    u8 padB1C[0xb2a - 0xb1c];
    s16 rewardId;
    u8 padB2C[2];
    s16 scriptParam;
    u8 padB30[8];
    u8 variant;
    u8 padB39[0xb4c - 0xb39];
    u32 defeatScript;
};

struct ArmorDefeatActorScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};

extern u32 gUnk_02000560;


extern const u8 gUnk_0824F004[];

void sub_08159164(s32);

void GameState_SetEventFlag(u32);

void Script_Call(u32, void *);

void Task_Kill(void *);

#define H(p, o) (*(s16 *)((p) + (o)))

static inline u32 Armor_TestDefeatBlocked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}

void Armor_FinalizeDefeat(struct ArmorDefeatActor *p)
{
    s32 values[8];
    struct ArmorDefeatActorScriptArgs args;
    u32 mask = 0x400;
    if (!((gEngineFlags | gScenePauseFlags) & mask) && !Armor_TestDefeatBlocked()) {
        s32 v;
        u32 script;
        sub_08159164(p->rewardId);
        H(gGameState, 0x87c) = H(gGameState, 0x87c) < 9999 ? H(gGameState, 0x87c) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        H(gGameState, 0x57e) = H(gGameState, 0x57e) < 9999 ? H(gGameState, 0x57e) + 1 : 9999;
        v = H(gGameState, 0x58c) + 1;
        if (v > 9999)
            v = 9999;
        else if (v < 0)
            v = 0;
        H(gGameState, 0x58c) = v;
        GameState_SetEventFlag(gUnk_0824F004[p->variant]);
        script = p->defeatScript;
        if (script) {
            args.count = 8;
            args.values = values;
            values[0] = p->x;
            values[1] = p->y;
            values[2] = p->z;
            values[3] = 0;
            values[4] = 0;
            values[5] = 0;
            values[6] = p->scriptParam;
            values[7] = p->scriptValue;
            Script_Call(script, &args);
        }
    }
    Task_Kill(p);
}

struct ArmorDeathActor {
    u8 f0[0xB3D];
    u8 b3d;
    u8 f1[2];
    u8 b40;
};




void Armor_UpdateDeathSequence(struct ArmorDeathActor *p)
{
    switch (p->b3d) {
        case 0:
            ActorController_ForceAction(p, 4);
            p->b3d = 1;
            break;
        case 1:
            if (p->b40 == 0xff) {
                DirectionalActor_TryBeginState(p, 0x12);
                p->b3d = 2;
            }
            break;
        case 2:
            if (p->b40 == 0xff) {
                p->b3d = 0xff;
                Armor_FinalizeDefeat(p);
            }
            break;
    }
}
