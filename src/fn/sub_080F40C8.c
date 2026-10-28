#include "global.h"
#include "ram.h"
#define ChildGroup_UpdateVerticalOscillation Ivy_UpdateBobbing
#define gChildGroupRandomIndex               gRandomIndex
#define gChildGroupRandomTable               gUnk_0203B400
struct ChildGroup {
    u8 pad0[0x8D];
    u8 flags;
    u8 pad8E[0x14];
    u8 reset;
    u8 padA3[0xC41];
    void (*callbackCE4)(struct ChildGroup *);
    void (*callbackCE8)(struct ChildGroup *);
    u8 padCEC[0x3C];
    u16 timerD28, timerD2A;
    u8 count, randomByte;
    u8 padD2E[0x27];
    u8 mode;
};
extern u16 gUnk_0203B400[];
void sub_080F33BC(struct ChildGroup *);
void Sword_UpdateSpinningLungeAttack(struct ChildGroup *);
void Sword_UpdateThrustAttack(struct ChildGroup *);
void Sword_UpdateWaveAttack(struct ChildGroup *);
void Ivy_UpdateBobbing(struct ChildGroup *);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
static inline u8 EnterState(struct ChildGroup *p)
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
void ChildGroup_UpdateAlternatingCallbacks(struct ChildGroup *p)
{
    if (EnterState(p)) {
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
        p->callbackCE4 = sub_080F33BC;
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
