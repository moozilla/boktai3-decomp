#include "global.h"
struct PhaseSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6, pad7;
    s8 scaleX, scaleY;
};
struct PhaseCollision {
    u8 pad0[6];
    u16 flags;
};
struct PhaseActor {
    u8 pad0[4];
    struct PhaseSprite *sprite;
    u8 pad8[0xfc];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x1c];
    struct PhaseCollision collision134;
    u8 pad13c[0x4c];
    struct PhaseCollision collision188;
    u8 pad190[0x8c];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0xa];
    u16 flags2c0;
    u8 pad2c2[6];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};
static inline u8 consumeEntry(struct PhaseActor *s)
{
    if (s->entered) {
        s->entered = 0;
        return 1;
    }
    return 0;
}
static inline void clearFlags(u32 *p, u32 mask)
{
    *p &= ~mask;
}

#define ActorPhase_SetAnimation Axe_SetAuxiliarySpriteParameters
u8 ActorPhase_UpdateReverseFlickerTimer(u32 *, s32, s32, s32);
void Axe_SetAuxiliarySpriteParameters(struct PhaseActor *, void *, u32);
void ActorPhase_UpdateReverseFlickerExit(void);
void ActorPhase_UpdateReverseFlicker(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    u32 flags;
    if (consumeEntry(s)) {
        data[0xcab] = 0;
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
    if (ActorPhase_UpdateReverseFlickerTimer(&flags, frame, 10, 30)) {
        void (*next)(void) = ActorPhase_UpdateReverseFlickerExit;
        u32 id = 0x30;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        goto visible;
    } else if (flags & 1) {
    visible:
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
}
