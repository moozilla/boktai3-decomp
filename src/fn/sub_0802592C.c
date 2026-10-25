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
    u32 w;
    u32 h;
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
    struct Sub0802592CPlayer *unk10C[69];
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
    s8 unk395;
    u8 unk396;
    u8 unk397;
    u8 unk398;
    u8 unk399;
    u8 unk39A;
    u8 unk39B[0x3AC - 0x39B];
    u32 unk3AC;
    u8 unk3B0[0x9CC - 0x3B0];
    u32 unk9CC;
    u32 unk9D0;
};

#ifdef NONMATCHING
// fuzzy: 59.2%
// scope shape 120 (differences once low registers are set aside)
// confidence=low: player-trail update: a per-frame state machine that starts an animation on a flagged frame, copies a position pair, and reports health and damage changes
void sub_0802592C(struct Sub0802592CActor *actor, s32 param)
{
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
    u32 *pair;

    if (actor->unkB1) {
        actor->unkB1 = 0;
        idx = actor->unk395;
        state = gUnknown_02000580[idx]->unk418;
        switch (state) {
            case 1:
                animA = 0x14;
                animB = 0x3A;
                break;
            case 2:
                animA = 0x15;
                animB = 0x3B;
                break;
            default:
                animA = 0x13;
                animB = 0x39;
                break;
        }
        if (actor->unk3AC & 1)
            anim = animA;
        else
            anim = animB;
        SceneActors_StartAnimation(actor, (u8 *)actor + 0x240, (u8 *)actor + 0x220, anim, 1, 4);
        if (actor->unkA8 > 1) {
            actor->unk51 = 0x40;
            actor->unkA8 = 2;
        } else {
            actor->unk51 = 0;
            actor->unkA8 = 1;
        }
        actor->unkA9 = ((actor->unk51 + 0x70) >> 5) & 7;
        idx = actor->unk395;
        pair = (u32 *)actor->unk10C[idx + 1];
        actor->unk9CC = pair[0];
        actor->unk9D0 = pair[1];
        Player_BeginPositionRequest(gUnknown_02000580[idx], (u8 *)actor + 0x260, 0, 0, 0, 0, 1, 0,
                                    2);
        idx = actor->unk395;
        if (idx >= 0) {
            v = actor->unk10C[idx]->unk4F4;
            actor->unk396 = v;
            actor->unk397 = v;
        }
        actor->unk399 = 0;
        actor->unk39A = 0;
    }

    cb = (u8)Callback_InvokeActor(actor, actor->unk104);
    if (cb != 0) {
        idx = actor->unk395;
        Player_ApplyPositionRequest(gUnknown_02000580[idx], &actor->unk9CC, 0x32, actor->unk51,
                                    0x32, 0, 0, 0);
        return;
    }

    cb2 = (u8)Callback_InvokeActor(actor, actor->unk108);
    if (cb2 != 0) {
        idx = actor->unk395;
        Player_ApplyPositionRequest(gUnknown_02000580[idx], &actor->unk9CC, 0x32, actor->unk51,
                                    0x32, 0, 0, 0);
        return;
    }

    idx = actor->unk395;
    q = actor->unk10C[idx];
    v = q->unk4F4;
    w = actor->unk397;
    if (w != v) {
        actor->unk397 = v;
        actor->unk39A++;
    }
    if (actor->unk39A > 3) {
        actor->unk398 = ((Player_GetInputDirection(q) << 29) + 0xA0000000) >> 24;
        actor->unk399 = 0x10;
        actor->unk39A = cb2;
    }
    if (actor->unk254 == 3 && actor->unk24C == 0) {
        reqRes = Player_BeginPositionRequest(gUnknown_02000580[actor->unk395], (u8 *)actor + 0x260,
                                             0, 0, 0, 0, 1, actor->unk368, 2);
        hp = actor->unk94;
        SceneActors_SetHealth(actor, hp + reqRes);
        if (hp != actor->unk94) {
            size.w = (size.w & 0xFFFF0000) | 0x18;
            size.w &= 0xFFFF;
            size.h = (size.h & 0xFFFF0000) | 0x18;
            DamageNumberDisplay_Show((u8 *)actor + 0x1CC, reqRes, 1, (u8 *)actor + 0x54, &size, 1);
        }
        sub_08022694(actor);
        Sound_PlaySongChecked(0x34C);
    }

    idx = actor->unk395;
    q = actor->unk10C[idx];
    if (((q->unk4F4 - (actor->unk396 - 0x100)) & 0xFF) > 0xF)
        actor->unk383 = 1;
    if (actor->unk25D & 2) {
        if ((gUnk_02000560 & 4) || actor->unk383 || param > 0x78)
            actor->unkAF = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0802592C);
#endif
