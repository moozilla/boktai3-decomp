#include "global.h"
#include "engine.h"
#include "ram.h"
void SceneActorAnimation_SelectProfileEntry(void *, s32);
void Anim_SetDurationScale(void *, u32);
void Slime_RollMoveFlag(void *);
void Slime_StartWalkChoice(void *);

struct Slime {
    u8 pad0[4];
    u8 *drawable;
    u8 pad8[0xfc];
    u32 field104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x104];
    void (*next)(void *);
    u8 pad220[0x90];
    u8 field2B0, pad2B1, prepared, enabled, pad2B4, wait;
    u8 pad2B6[0x12];
    u16 field2C8;
};

static inline u8 TakePrepared(struct Slime *p)
{
    if (p->prepared) {
        p->prepared = 0;
        return 1;
    }
    return 0;
}

static inline void ClearFlags(u32 *flags, u32 mask)
{
    u32 m = ~mask;
    *flags &= m;
}

static inline void SetNext(struct Slime *p, void (*fn)(void *), u32 wait)
{
    u8 *flag = &p->prepared;
    u32 zero = 0;
    *flag = 1;
    p->field2B0 = zero;
    p->wait = wait;
    p->next = fn;
    p->field2C8 = zero;
    ClearFlags(&p->flags114, 1);
    p->field104 = zero;
}

void sub_0808EB98(struct Slime *p)
{
    if (TakePrepared(p)) {
        SceneActorAnimation_SelectProfileEntry(p, 0xe);
        Anim_SetDurationScale(p->drawable + 0x48, 0x40);
    }
    if (p->enabled != 0) {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 r;
        gRandomIndex = index;
        r = *(u16 *)(index * 2 + (u32)gRandomTable);
        r &= 0x1f;
        if (r <= 0x17)
            SetNext(p, Slime_RollMoveFlag, 0);
        else
            SetNext(p, Slime_StartWalkChoice, 0x29);
    }
}
