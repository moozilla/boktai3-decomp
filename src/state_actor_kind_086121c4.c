#include "state_actor.h"

void Actor_DispatchPrimaryState(u32, u32);

void StateActor_DispatchCommand(u32 a, u32 b)
{
    Actor_DispatchPrimaryState(a, b);
}

void StateActor_DispatchCommand(u32, u32);

s32 StateActor_OnCommand(u32 a, u32 b)
{
    StateActor_DispatchCommand(a, b);
    return 0;
}

extern u32 gUnk_0861218C[];

s32 StateActor_EnterPendingState(struct StateActor *p)
{
    s32 r;
    if ((u8)(p->nextState - 1) <= 0xd) {
        u32 t = gUnk_0861218C[p->nextState];
        u8 v = p->nextState;
        s32 z = 0;
        p->prevState = v;
        p->stateCallback = (void (*)(void))t;
        p->stateFrame = z;
        p->enter = 1;
        p->phase = z;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}

u32 StateActor_OnUpdateState(void *a, struct StateActor *p)
{
    u32 n;
    void (*fn)(void *, void *, u32);
    p->displayFlags = 0;
    p->done = 0;
    n = p->stateFrame++;
    fn = (void (*)(void *, void *, u32))p->stateCallback;
    if (fn)
        fn(a, p, n);
    return 0;
}

INCLUDE_ASM("asm/nonmatching", sub_081C6780);

void sub_081C6780(struct StateActor *);

void Actor_UpdateMovementAnimationAndEffects(u32, struct StateActor *);

s32 StateActor_OnUpdateDisplay(u32 a, struct StateActor *b)
{
    s32 z;
    sub_081C6780(b);
    Actor_UpdateMovementAnimationAndEffects(a, b);
    z = 0;
    b->nextState = z;
    return z;
}

void Actor_UpdateVisibilityAndShadow(u32, u32);

s32 StateActor_OnRender(u32 a, u32 b)
{
    Actor_UpdateVisibilityAndShadow(a, b);
    return 0;
}

u32 SceneMessages_UnregisterReceiver(u8 *);

u32 StateActor_OnUnregisterReceiver(u32 a, struct StateActor *b)
{
    return SceneMessages_UnregisterReceiver(b->receiver);
}

INCLUDE_ASM("asm/nonmatching", sub_081C6954);

extern void *gUnk_02000480;

u32 Script_GetKeywordValue(u32, u32);

void sub_081C6954(void *, u32);

void StateActor_CreateFromScript(void)
{
    void *p = gUnk_02000480;
    if (p != 0)
        sub_081C6954(p, Script_GetKeywordValue(0x6e, 0));
}

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);

static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}

s32 DisplayAnimation_TestFinished(void *);

void StateActor_UpdateAnimation4State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        {
            u32 z = 0;
            p->phase = z;
            p->stateFrame = z;
        }
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void StateActor_UpdateAnimation5State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 5;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C6D28);

void StateActor_UpdateAnimation10State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 10;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C7024);
INCLUDE_ASM("asm/nonmatching", sub_081C725C);
INCLUDE_ASM("asm/nonmatching", sub_081C73D0);
INCLUDE_ASM("asm/nonmatching", sub_081C74F0);

void StateActor_UpdateAnimation15State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0xf;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
    if (p->phase == 0) {
        if (DisplayAnimation_TestFinished(p->animation) != 0) {
            Actor_PlayAnimationEntry(p, 0, 2, 0, 4);
            p->done = 1;
            p->phase = 1;
        }
    }
}

INCLUDE_ASM("asm/nonmatching", sub_081C7934);
INCLUDE_ASM("asm/nonmatching", sub_081C7A9C);
INCLUDE_ASM("asm/nonmatching", sub_081C7CC0);

void StateActor_UpdateAnimation20State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x14;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0) {
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
        p->done = 1;
    }
}

INCLUDE_ASM("asm/nonmatching", sub_081C7F00);

void StateActor_UpdateAnimation21State(u32 a, struct StateActor *p, s32 n)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x15;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
    if (n > 10)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C80D0);

void StateActor_StartAnimation23State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x17;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }
    p->done = 1;
}

void StateActor_UpdateAnimation24State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x18;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 3, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void StateActor_StartAnimation25State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x19;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }
    p->done = 1;
}

void StateActor_UpdateAnimation26State(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                r = 0;
                break;
            case 27:
                r = 0x1a;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 3, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

struct P {
    u32 a;
    u32 b;
};

void SunOrb_GetOrCreateManager(void);

u32 SunOrb_CreatePlayerTargeted(void *);

void Sound_PlaySong(u32);

void StateActor_SpawnRaisedEffect(u32 a, struct StateActor *p)
{
    u8 *e = (u8 *)p + 0x20;
    if (p->enter != 0) {
        struct P *d;
        u16 *h;
        p->enter = 0;
        p->done = 0;
        SunOrb_GetOrCreateManager();
        d = (struct P *)((u8 *)p + 0x54);
        *d = *(struct P *)((u8 *)p + 0x68);
        h = (u16 *)((u8 *)p + 0x56);
        *h = (u16)(0x100 + *h);
        *(u32 *)(e + 4) = SunOrb_CreatePlayerTargeted(d);
        Sound_PlaySong(0x571);
    }
    p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C84CC);
INCLUDE_ASM("asm/nonmatching", sub_081C85F4);
INCLUDE_ASM("asm/nonmatching", sub_081C879C);

void StateActor_StartKind28Animation4(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 29:
                r = 0;
                break;
            case 28:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    p->done = 1;
}

void Actor_UpdateKindTwentyEightEntryFive(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 29:
                r = 0;
                break;
            case 28:
                r = 5;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 3, 0, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C8A60);
INCLUDE_ASM("asm/nonmatching", sub_081C8C8C);
INCLUDE_ASM("asm/nonmatching", sub_081C8EC0);
INCLUDE_ASM("asm/nonmatching", sub_081C8FB0);

u32 Actor_UpdateAcceleratingBlink(struct StateActor *p, s32 b, s32 c)
{
    s32 n;
    if (b > (c * (c + 1)) >> 1)
        return 1;
    n = 0;
    if (n < c) {
        b -= c;
        if (b > 0) {
            do {
                n++;
                if (n >= c)
                    break;
                b -= c - n;
            } while (b > 0);
        }
    }
    if ((n & 1) == 0) {
        u32 m = 0x100;
        p->displayFlags = m | p->displayFlags;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatching", sub_081C90E0);
INCLUDE_ASM("asm/nonmatching", sub_081C922C);
INCLUDE_ASM("asm/nonmatching", sub_081C93A0);

void Actor_UpdateKindNineteenEntryFour(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 19:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->displayFlags = m | p->displayFlags;
    }
}

void Actor_UpdateKindNineteenEntrySix(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 19:
                r = 6;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->displayFlags = m | p->displayFlags;
    }
}

void Actor_UpdateKindNineteenEntrySeven(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 19:
                r = 7;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->displayFlags = m | p->displayFlags;
    }
}

void Actor_UpdateKindTwentyPairEntryFour(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 20:
            case 21:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->displayFlags = m | p->displayFlags;
    }
}

void Actor_UpdateKindFourEntryFour(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFourEntryFourFlagsTwo(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 2);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFourEntryFive(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 5;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFourEntryFiveFlagsSix(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 5;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 6);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFourEntrySeven(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 7;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081C9BA0);

void Actor_UpdateKindFourEntryTen(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 4:
                r = 0xa;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_BeginKindFourEntrySix(void *unused, struct StateActor *p)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 6;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 0;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
}

void Actor_UpdateKindFiveEntryFour(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 5:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFiveEntryFourFlagsSix(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 5:
                r = 4;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 6);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

void Actor_UpdateKindFiveEntryFive(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 5:
                r = 5;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081CA0A0);

void Actor_UpdateKindFiveEntryEight(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 5:
                r = 8;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081CA2C8);

void Actor_UpdateKindFiveEntryTen(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 5:
                r = 0xa;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081CA490);

void Actor_BeginKindFifteenEntryNine(void *unused, struct StateActor *p)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 9;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
}

void Actor_UpdateKindFifteenEntryTenTimed(void *unused, struct StateActor *p, s32 n)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 10;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
    }
    if (n > 15)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081CA9B8);

void SceneCharacter_UpdateAnimationFiveSixFive(void *unused, struct StateActor *p)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 5;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation)) {
        switch (p->phase) {
            case 0: {
                s32 r;
                switch (p->kind) {
                    case 4:
                        r = 0;
                        break;
                    case 5:
                        r = 0;
                        break;
                    case 6:
                        r = 0;
                        break;
                    case 7:
                        r = 0;
                        break;
                    case 8:
                        r = 0;
                        break;
                    case 9:
                        r = 0;
                        break;
                    case 10:
                        r = 0;
                        break;
                    case 11:
                        r = 0;
                        break;
                    case 12:
                        r = 0;
                        break;
                    case 13:
                        r = 0;
                        break;
                    case 14:
                        r = 0;
                        break;
                    case 15:
                        r = 6;
                        break;
                    case 16:
                        r = 0;
                        break;
                    case 17:
                        r = 0;
                        break;
                    case 18:
                        r = 0;
                        break;
                    case 19:
                        r = 0;
                        break;
                    case 20:
                        r = 0;
                        break;
                    case 21:
                        r = 0;
                        break;
                    case 22:
                        r = 0;
                        break;
                    case 23:
                        r = 0;
                        break;
                    case 24:
                        r = 0;
                        break;
                    case 25:
                        r = 0;
                        break;
                    case 26:
                        r = 0;
                        break;
                    case 27:
                        r = 0;
                        break;
                    case 28:
                        r = 0;
                        break;
                    case 29:
                        r = 0;
                        break;
                    default:
                        r = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->phase++;
                break;
            }
            case 1: {
                s32 r;
                switch (p->kind) {
                    case 4:
                        r = 0;
                        break;
                    case 5:
                        r = 0;
                        break;
                    case 6:
                        r = 0;
                        break;
                    case 7:
                        r = 0;
                        break;
                    case 8:
                        r = 0;
                        break;
                    case 9:
                        r = 0;
                        break;
                    case 10:
                        r = 0;
                        break;
                    case 11:
                        r = 0;
                        break;
                    case 12:
                        r = 0;
                        break;
                    case 13:
                        r = 0;
                        break;
                    case 14:
                        r = 0;
                        break;
                    case 15:
                        r = 7;
                        break;
                    case 16:
                        r = 0;
                        break;
                    case 17:
                        r = 0;
                        break;
                    case 18:
                        r = 0;
                        break;
                    case 19:
                        r = 0;
                        break;
                    case 20:
                        r = 0;
                        break;
                    case 21:
                        r = 0;
                        break;
                    case 22:
                        r = 0;
                        break;
                    case 23:
                        r = 0;
                        break;
                    case 24:
                        r = 0;
                        break;
                    case 25:
                        r = 0;
                        break;
                    case 26:
                        r = 0;
                        break;
                    case 27:
                        r = 0;
                        break;
                    case 28:
                        r = 0;
                        break;
                    case 29:
                        r = 0;
                        break;
                    default:
                        r = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->phase++;
                break;
            }
            case 2:
                p->done = 1;
                p->phase++;
                break;
            default:
                return;
        }
    }
}

void SceneCharacter_StartSlowAnimationSeven(void *unused, struct StateActor *p)
{
    if (p->enter) {
        int r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 15:
                r = 7;
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 1);
        p->phase = 0;
    }
}

void SceneCharacter_StartAnimationSeven(void *unused, struct StateActor *p)
{
    if (p->enter) {
        int r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 15:
                r = 7;
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
}

void SceneCharacter_UpdateAnimationSevenSixFive(void *unused, struct StateActor *p)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 7;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation)) {
        switch (p->phase) {
            case 0: {
                s32 r;
                switch (p->kind) {
                    case 4:
                        r = 0;
                        break;
                    case 7:
                        r = 0;
                        break;
                    case 6:
                        r = 0;
                        break;
                    case 5:
                        r = 0;
                        break;
                    case 8:
                        r = 0;
                        break;
                    case 9:
                        r = 0;
                        break;
                    case 10:
                        r = 0;
                        break;
                    case 11:
                        r = 0;
                        break;
                    case 12:
                        r = 0;
                        break;
                    case 13:
                        r = 0;
                        break;
                    case 14:
                        r = 0;
                        break;
                    case 15:
                        r = 6;
                        break;
                    case 16:
                        r = 0;
                        break;
                    case 17:
                        r = 0;
                        break;
                    case 18:
                        r = 0;
                        break;
                    case 19:
                        r = 0;
                        break;
                    case 20:
                        r = 0;
                        break;
                    case 21:
                        r = 0;
                        break;
                    case 22:
                        r = 0;
                        break;
                    case 23:
                        r = 0;
                        break;
                    case 24:
                        r = 0;
                        break;
                    case 25:
                        r = 0;
                        break;
                    case 26:
                        r = 0;
                        break;
                    case 27:
                        r = 0;
                        break;
                    case 28:
                        r = 0;
                        break;
                    case 29:
                        r = 0;
                        break;
                    default:
                        r = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->phase++;
                break;
            }
            case 1: {
                s32 r;
                switch (p->kind) {
                    case 4:
                        r = 0;
                        break;
                    case 7:
                        r = 0;
                        break;
                    case 6:
                        r = 0;
                        break;
                    case 5:
                        r = 0;
                        break;
                    case 8:
                        r = 0;
                        break;
                    case 9:
                        r = 0;
                        break;
                    case 10:
                        r = 0;
                        break;
                    case 11:
                        r = 0;
                        break;
                    case 12:
                        r = 0;
                        break;
                    case 13:
                        r = 0;
                        break;
                    case 14:
                        r = 0;
                        break;
                    case 15:
                        r = 5;
                        break;
                    case 16:
                        r = 0;
                        break;
                    case 17:
                        r = 0;
                        break;
                    case 18:
                        r = 0;
                        break;
                    case 19:
                        r = 0;
                        break;
                    case 20:
                        r = 0;
                        break;
                    case 21:
                        r = 0;
                        break;
                    case 22:
                        r = 0;
                        break;
                    case 23:
                        r = 0;
                        break;
                    case 24:
                        r = 0;
                        break;
                    case 25:
                        r = 0;
                        break;
                    case 26:
                        r = 0;
                        break;
                    case 27:
                        r = 0;
                        break;
                    case 28:
                        r = 0;
                        break;
                    case 29:
                        r = 0;
                        break;
                    default:
                        r = 0;
                        break;
                }
                Actor_PlayAnimationEntry(p, r, 2, 0, 4);
                p->phase++;
                break;
            }
            case 2:
                p->done = 1;
                p->phase++;
                break;
            default:
                return;
        }
    }
}

void SceneCharacter_UpdateAnimationFiveToIdle(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
                r = 0;
                break;
            case 15:
                r = 5;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 0, 4);
        p->phase = 0;
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0) {
        switch (p->phase) {
            case 0:
                Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
                break;
            case 1:
                p->done = 1;
                break;
            default:
                return;
        }
        p->phase++;
    }
}

INCLUDE_ASM("asm/nonmatching", sub_081CB174);
INCLUDE_ASM("asm/nonmatching", sub_081CB294);
INCLUDE_ASM("asm/nonmatching", sub_081CB3F4);
INCLUDE_ASM("asm/nonmatching", sub_081CB5B0);
INCLUDE_ASM("asm/nonmatching", sub_081CB6AC);

void SceneCharacter_UpdateAnimationThirteen(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xd;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 0, 1, 2);
        {
            u32 z = 0;
            p->phase = z;
            p->stateFrame = z;
        }
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

extern u8 *gUnk_030042E4;

s32 Mod(s32, s32);

void Palette_InterpolateBanks(void *dst, void *from, void *to, s32 t, s32 shift);

/* Pulse the palette between the 0x680 and 0x2E80 banks: 64 frames each way. */

void SceneCharacter_UpdatePalettePulse(u8 *a, struct StateActor *b)
{
    s32 t;
    if (b->enter) {
        u32 zero = 0;
        b->enter = zero;
        b->phase = zero;
        b->stateFrame = zero;
        b->done = 1;
    }
    t = Mod(b->stateFrame, 128);
    if (t <= 64) {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x680, *to = bank + 0x2e80;
        Palette_InterpolateBanks((u8 *)b + 0x1cc, from, to, t, 6);
    } else {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x2e80, *to = bank + 0x680;
        Palette_InterpolateBanks((u8 *)b + 0x1cc, from, to, t - 64, 6);
    }
}

/* Copy the 0x680 palette bank in, then pulse between it and the 0x2DE0 bank. */

void SceneCharacter_UpdateAlternatePalettePulse(u8 *a, struct StateActor *b)
{
    s32 t;
    if (b->enter) {
        u32 zero = 0;
        b->enter = zero;
        b->done = zero;
        CpuSet(gUnk_030042E4 + 0x680, (u8 *)b + 0x1cc, 0x04000008);
        b->phase = zero;
        b->stateFrame = zero;
        b->done = 1;
    }
    t = Mod(b->stateFrame, 128);
    if (t <= 64) {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x680, *to = bank + 0x2de0;
        Palette_InterpolateBanks((u8 *)b + 0x1cc, from, to, t, 6);
    } else {
        u8 *bank = gUnk_030042E4;
        u8 *from = bank + 0x2de0, *to = bank + 0x680;
        Palette_InterpolateBanks((u8 *)b + 0x1cc, from, to, t - 64, 6);
    }
}

#ifdef NONMATCHING

// fuzzy: 47.1%

// register allocation: the original keeps pos/target/heading/velocity in r6/r8/sb/sl

struct Vec081CBB1C {
    s16 x, y, z, pad;
};

struct Pair081CBB1C {
    u32 a, b;
};

extern const s16 gUnk_086149C4[];

u8 Math_Atan2(s32, s32);

static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

/* Walk toward the target at +0x54: snap and finish when within the squared
 * radius at +0x20C, else step along the heading and turn toward the target. */

void sub_081CBB1C(u8 *a, u8 *b)
{
    struct Vec081CBB1C *pos, *target, *vel;
    u8 *heading;
    s32 dx, dz;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
    }
    pos = (struct Vec081CBB1C *)(b + 0x68);
    target = (struct Vec081CBB1C *)(b + 0x54);
    dx = target->x - pos->x;
    dz = target->z - pos->z;
    if ((u32)(dx * dx + dz * dz) <= *(u32 *)(b + 0x20c)) {
        *(struct Pair081CBB1C *)pos = *(struct Pair081CBB1C *)target;
        b[7] = 1;
        return;
    }
    heading = b + 0x65;
    vel = (struct Vec081CBB1C *)(b + 0x70);
    {
        u8 angle = *heading;
        s32 speed = *(s32 *)(b + 0x208);
        vel->x = shift12(speed * gUnk_086149C4[(angle + 64) & 255]);
        vel->y = 0;
        vel->z = shift12(speed * gUnk_086149C4[angle]);
    }
    b[0x1b] = Math_Atan2(target->x - pos->x, target->z - pos->z);
    *heading = b[0x1b];
    b[0x1a] = ((*heading + 0x70) >> 5) & 7;
    b[0x19] = ((*heading + 0x60) >> 6) & 3;
    pos->x += vel->x;
    pos->z += vel->z;
}

#else

INCLUDE_ASM("asm/nonmatching", sub_081CBB1C);

#endif

struct Pair081CBC44 {
    u32 a, b;
};

struct Pair081CBC44 *SunOrb_GetPosition(s32);

void sub_081FED40(s32);

void sub_081C8EC0(struct StateActor *);

void SceneCharacter_UpdateParticleDissolve(u8 *a, struct StateActor *b, s32 frame)
{
    u8 *pos = (u8 *)b + 0x20;
    if (b->enter) {
        u32 zero = 0;
        b->enter = zero;
        b->done = zero;
        *(struct Pair081CBC44 *)(pos + 8) = *SunOrb_GetPosition(*(s16 *)(pos + 4));
        sub_081FED40(*(s16 *)(pos + 4));
        Sound_PlaySong(0x5ca);
        b->phase = zero;
        b->stateFrame = zero;
    }
    if (frame <= 30) {
        if (frame <= 19 || Mod(frame, 2) == 0) {
            sub_081C8EC0(b);
            if (frame <= 9) {
                s32 i = 1;
                do {
                    sub_081C8EC0(b);
                    i--;
                } while (i >= 0);
            }
        }
    }
    if (frame > 30)
        b->done = 1;
}

void SceneCharacter_StartAnimationEleven(void *unused, struct StateActor *p)
{
    if (p->enter) {
        s32 r;
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 0;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 0;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 11;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        p->done = 1;
    }
}

void SceneCharacter_UpdateAnimationSeven(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 7;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
        {
            u32 z = 0;
            p->phase = z;
            p->stateFrame = z;
        }
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

#ifdef NONMATCHING

// fuzzy: 79.5%

// register allocation: b/frame/state get r5/r6/r4 instead of r6/r4/r5

struct Vec081CBE30 {
    s16 x, y, z, pad;
};

void TimedSpriteEffect_Create(struct Vec081CBE30 *, s32, s32, s32, s32, s32, s32, s32, s32);

/* Three-phase attack: play the wind-up, spawn an effect above the actor on
 * frame 15, then the follow-through after 0x56 frames. */

void sub_081CBE30(u8 *a, u8 *b, s32 frame)
{
    u32 anim;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        switch (b[1]) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                anim = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                anim = 0;
                break;
            case 29:
                anim = 8;
                break;
            default:
                anim = 0;
                break;
        }
        Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
        {
            u32 zero = 0;
            b[0x18] = zero;
            *(u32 *)(b + 0x10) = zero;
        }
    }
    switch (b[0x18]) {
        case 0:
            if (frame == 15) {
                s16 *p = (s16 *)(b + 0x68);
                s16 v[4];
                s32 x = *p++;
                s32 y = *p++;
                s32 z = *p;
                v[0] = x - 0x20;
                v[1] = y + 0x180;
                v[2] = z + 0x20;
                TimedSpriteEffect_Create((struct Vec081CBE30 *)v, 0xa0, 0x20, 1, 1, 0, 0, 0, 0);
            }
            if (DisplayAnimation_TestFinished(b + 0xd4))
                b[0x18] = 1;
            break;
        case 1:
            if (*(u32 *)(b + 0x10) > 0x56) {
                b[0x18] = 2;
                switch (b[1]) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                        anim = 0;
                        break;
                    case 16:
                    case 17:
                    case 18:
                    case 19:
                    case 20:
                    case 21:
                    case 22:
                    case 23:
                    case 24:
                    case 25:
                    case 26:
                    case 27:
                    case 28:
                        anim = 0;
                        break;
                    case 29:
                        anim = 9;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(b, anim, 2, 1, 4);
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(b + 0xd4))
                b[7] = 1;
            break;
    }
}

#else

INCLUDE_ASM("asm/nonmatching", sub_081CBE30);

#endif

void SceneCharacter_UpdateAnimationTen(u32 a, struct StateActor *p)
{
    u32 r;
    if (p->enter != 0) {
        p->enter = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                r = 0;
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xa;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 2, 1, 4);
        {
            u32 z = 0;
            p->phase = z;
            p->stateFrame = z;
        }
    }
    if (DisplayAnimation_TestFinished(p->animation) != 0)
        p->done = 1;
}

INCLUDE_ASM("asm/nonmatching", sub_081CC0C8);
INCLUDE_ASM("asm/nonmatching", sub_081CC1B4);

void SceneCharacter_StartAnimationThree(u8 *a, struct StateActor *b)
{
    if (b->enter) {
        u32 z = 0;
        b->enter = z;
        b->done = z;
        Actor_PlayAnimationEntry(b, 3, 1, 0, 4);
        b->phase = z;
        b->stateFrame = z;
        b->done = 1;
    }
}

#ifdef NONMATCHING

// fuzzy: 92.0%

// dead movs r5, #0 placement and the store-flag of the flash test

typedef u32 (*ActorUpdate)(u8 *);

s32 sub_081C8FB0(u8 *, u8 *, u32);

static inline u8 Actor_ShouldFlash(u8 *a, u8 *b)
{
    u8 hit = 0;
    if ((u8)(*(ActorUpdate *)(b + 0x2e4))(b) != 1 && sub_081C8FB0(a, b, 0))
        hit = 1;
    return hit;
}

void sub_081CC4CC(u8 *a, u8 *b)
{
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        Actor_PlayAnimationEntry(b, 0, 1, 0, 4);
    }
    orr((u16 *)(b + 0xe), 0x1000);
    if (Actor_ShouldFlash(a, b))
        orr((u16 *)(b + 0xe), 0x800);
}

#else

INCLUDE_ASM("asm/nonmatching", sub_081CC4CC);

#endif

void sub_0802F298(void);

void SceneCharacter_FaceSceneTarget(void)
{
    sub_0802F298();
}

/* Blink for 15 frames: bit 0 of the phase sets display flag 0x100, and each
 * phase lasts one frame longer than the last. */

void SceneCharacter_UpdateSlowingBlink(u8 *a, struct StateActor *b)
{
    u32 t;
    u8 phase;
    if (b->enter) {
        u32 one;
        b->enter = 0;
        b->done = 0;
        one = 1;
        b->timer = one;
        b->phase = one;
    }
    t = b->timer;
    if (t > 15) {
        b->done = 1;
        return;
    }
    phase = b->phase;
    if (phase & 1)
        orr(&b->displayFlags, 0x100);
    if (t != 0) {
        u32 left = t - 1;
        b->timer = left;
        if (left == 0) {
            b->phase = phase + 1;
            b->timer = b->phase;
        }
    }
}

u32 Actor_UpdateAcceleratingBlink(struct StateActor *, s32, s32);

void SceneCharacter_UpdateAcceleratingBlink(u8 *a, struct StateActor *b, u32 c)
{
    if (b->enter) {
        b->enter = 0;
        b->done = 0;
        b->timer = 0x10;
    }
    if (Actor_UpdateAcceleratingBlink(b, c, 0x10)) {
        orr(&b->displayFlags, 0x100);
        b->done = 1;
    }
}

/* Blink forever: toggle the phase every 16 frames; odd phases set display
 * flag 0x100. */

void SceneCharacter_UpdateContinuousBlink(u8 *a, struct StateActor *b)
{
    u8 phase;
    u32 t;
    if (b->enter) {
        b->enter = 0;
        b->done = 0;
        b->timer = 0x10;
    }
    phase = b->phase;
    if (phase & 1)
        orr(&b->displayFlags, 0x100);
    t = b->timer;
    if (t != 0) {
        t--;
        b->timer = t;
        if (t == 0) {
            b->phase = (phase + 1) & 1;
            b->timer = 0x10;
        }
    }
}

extern u32 gUnk_086121C4[];

s32 SceneCharacter_OnStateChange(struct StateActor *p)
{
    s32 r;
    if ((u8)(p->nextState - 1) <= 0x5F) {
        u32 t = gUnk_086121C4[p->nextState];
        u8 v = p->nextState;
        s32 z = 0;
        p->prevState = v;
        p->stateCallback = (void (*)(void))t;
        p->stateFrame = z;
        p->enter = 1;
        p->phase = z;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}

#ifdef NONMATCHING

// fuzzy: 96.9%

// the two zero stores load their constants into swapped registers

typedef void (*ActorUpdateVoid)(u8 *);

typedef void (*ActorStateCallback)(u8 *, u8 *, u32);

/* Per-frame actor tick: clear the display flags, advance the frame counter,
 * run the update hook, then the current state's callback with the old count. */

s32 sub_081CC64C(u8 *a, u8 *b)
{
    u32 frame;
    ActorStateCallback cb;
    u8 zero8 = 0;
    *(u16 *)(b + 0xe) = 0;
    b[7] = zero8;
    frame = (*(u32 *)(b + 0x10))++;
    (*(ActorUpdateVoid *)(b + 0x2e4))(b);
    cb = *(ActorStateCallback *)(b + 0x2cc);
    if (cb)
        cb(a, b, frame);
    return 0;
}

#else

INCLUDE_ASM("asm/nonmatching", sub_081CC64C);

#endif
