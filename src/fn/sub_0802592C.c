#include "global.h"

extern struct Sub0802592CPlayer *gUnknown_02000580[];
extern u32 gUnk_02000560;

s32 Callback_InvokeActor(void *arg, void *function);
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
s32 Player_BeginPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32, u32);
s32 Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32);
void SceneActors_SetHealth(void *, s32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void sub_08022694(void *);
void Sound_PlaySongChecked(u32);
u32 Player_GetInputDirection(void *);

struct Sub0802592CPlayer {
    u8 unk0[0x418];
    u8 unk418;
    u8 unk419[0x4F4 - 0x419];
    u8 unk4F4;
};

struct Sub0802592CSize {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct Sub0802592CPair {
    u32 first, second;
};

struct Sub0802592CActor {
    u8 unk00[0x51];
    u8 unk51;
    u8 unk52[0x94 - 0x52];
    s16 unk94;
    u8 unk96[0xA8 - 0x96];
    u8 unkA8;
    u8 unkA9;
    u8 unkAA[0xAF - 0xAA];
    u8 unkAF;
    u8 unkB0;
    u8 unkB1;
    u8 unkB2[0x104 - 0xB2];
    void *unk104;
    void *unk108;
    union {
        struct Sub0802592CPlayer *players[69];
        struct {
            u32 unknown;
            struct Sub0802592CPair *positions[68];
        } references;
    } targets;
    u8 unk220[0x24C - 0x220];
    u16 unk24C;
    u8 unk24E[0x254 - 0x24E];
    u16 unk254;
    u8 unk256[0x25D - 0x256];
    u8 unk25D;
    u8 unk25E[0x368 - 0x25E];
    s16 unk368;
    u8 unk36A[0x383 - 0x36A];
    u8 unk383;
    u8 unk384[0x395 - 0x384];
    u8 unk395;
    u8 unk396;
    u8 unk397;
    u8 unk398;
    u8 unk399;
    u8 unk39A;
    u8 unk39B[0x3AC - 0x39B];
    u32 unk3AC;
    u8 unk3B0[0x9CC - 0x3B0];
    struct Sub0802592CPair saved;
};

/* Track the selected player and apply contact health changes. */
static inline u8 SceneTrail_TargetIndex(struct Sub0802592CActor *p)
{
    return p->unk395;
}

static inline void SceneTrail_CopySavedPosition(struct Sub0802592CActor *actor)
{
    s32 idx;
    u32 *pair;
    idx = (s8)SceneTrail_TargetIndex(actor);
    pair = (u32 *)actor->targets.references.positions[idx];
    *(struct Sub0802592CPair *)((u8 *)actor + 0x9cc) = *(struct Sub0802592CPair *)pair;
}

static inline s32 SceneTrail_WrappedDifference(s32 current, s32 previous)
{
    s32 before = previous - 0x100;
    return (current - before) & 0xff;
}

static inline u8 SceneTrail_DirectionToAngle(u8 direction)
{
    return (((u32)direction << 29) + 0xA0000000) >> 24;
}

void sub_0802592C(struct Sub0802592CActor *base, s32 param)
{
    struct Sub0802592CActor *actor = base;
    struct Sub0802592CPlayer *q;
    struct Sub0802592CSize size;
    s32 idx;
    s32 state;
    s32 animA;
    s32 animB;
    s32 anim;
    s32 cb;
    s32 cb2;
    s32 reqRes;
    s32 hp;
    u32 v;
    u32 w;

    if (base->unkB1) {
        base->unkB1 = 0;
        idx = (s8)SceneTrail_TargetIndex(base);
        state = gUnknown_02000580[idx]->unk418;
        switch (state) {
            case 0:
            default:
                animA = 0x13;
                animB = 0x39;
                break;
            case 2:
                animA = 0x15;
                animB = 0x3B;
                break;
            case 1:
                animA = 0x14;
                animB = 0x3A;
                break;
        }
        if (actor->unk3AC & 1)
            anim = animA;
        else
            anim = animB;
        SceneActors_StartAnimation(base, (u8 *)actor + 0x240, (u8 *)actor + 0x220, anim, 1, 4);
        {
            u8 *facingPtr = &base->unkA8;
            s32 facing;
            if (*facingPtr <= 1) {
                u32 angle = 0;
                base->unk51 = angle;
                facing = 1;
            } else {
                u32 angle = 0x40;
                base->unk51 = angle;
                facing = 2;
            }
            *facingPtr = facing;
        }
        {
            s32 shift = (base->unk51 + 0x70) >> 5;
            s32 direction = 7;
            direction &= shift;
            {
                u8 *out = &base->unkA9;
                *out = direction;
            }
        }
        SceneTrail_CopySavedPosition(actor);
        idx = (s8)SceneTrail_TargetIndex(actor);
        Player_BeginPositionRequest(gUnknown_02000580[idx], (u8 *)actor + 0x260, 0, 0, 0, 0, 1, 0,
                                    2);
        if ((s8)actor->unk395 >= 0) {
            v = base->targets.players[(s8)actor->unk395]->unk4F4;
            actor->unk396 = v;
            actor->unk397 = v;
        }
        actor->unk399 = 0;
        actor->unk39A = 0;
    }

    cb = (u8)Callback_InvokeActor(base, base->unk104);
    if (cb != 0) {
        idx = (s8)SceneTrail_TargetIndex(actor);
        Player_ApplyPositionRequest(gUnknown_02000580[idx], &actor->saved, 0x32, actor->unk51, 0x32,
                                    0, 0, 0);
        return;
    }

    cb2 = (u8)Callback_InvokeActor(base, base->unk108);
    if (cb2 != 0) {
        idx = (s8)SceneTrail_TargetIndex(actor);
        Player_ApplyPositionRequest(gUnknown_02000580[idx], &actor->saved, 0x32, actor->unk51, 0x32,
                                    0, 0, 0);
        return;
    }

    {
        u8 sample;
        struct Sub0802592CPlayer **targets;
        s8 *index = (s8 *)&actor->unk395;
        idx = *index;
        {
            u32 offset = idx * 4;
            struct Sub0802592CPlayer **references = base->targets.players;
            q = *(struct Sub0802592CPlayer **)((u8 *)references + offset);
            sample = q->unk4F4;
            w = actor->unk397;
            targets = references;
        }
        if (w != sample) {
            actor->unk397 = sample;
            actor->unk39A++;
        }
        if (actor->unk39A > 3) {
            actor->unk398 = SceneTrail_DirectionToAngle(Player_GetInputDirection(q));
            actor->unk399 = 0x10;
            actor->unk39A = cb2;
        }
        if (actor->unk254 == 3 && actor->unk24C == 0) {
            idx = *index;
            reqRes = Player_BeginPositionRequest(gUnknown_02000580[idx], (u8 *)actor + 0x260, 0, 0,
                                                 0, 0, 1, actor->unk368, 2);
            hp = actor->unk94;
            SceneActors_SetHealth(actor, hp + reqRes);
            if (hp != actor->unk94) {
                void *contact = (u8 *)actor + 0x1cc;
                size.x = 0x18;
                size.y = 0;
                size.z = 0x18;
                DamageNumberDisplay_Show(contact, reqRes, 1, (u8 *)actor + 0x54, &size, 1);
            }
            sub_08022694(actor);
            Sound_PlaySongChecked(0x34C);
        }

        idx = (s8)SceneTrail_TargetIndex(actor);
        {
            struct Sub0802592CPlayer *currentPlayer = targets[idx];
            s32 previous = actor->unk396;
            s32 current = currentPlayer->unk4F4;
            if (SceneTrail_WrappedDifference(current, previous) > 0xf)
                actor->unk383 = 1;
        }
        if (actor->unk25D & 2) {
            u32 mask = 4;
            if ((gUnk_02000560 & mask) || actor->unk383 || param > 0x78)
                base->unkAF = 1;
        }
    }
}
