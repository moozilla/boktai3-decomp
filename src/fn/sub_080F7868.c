#include "global.h"
#define ActorPhase_SetAnimation Axe_SetAuxiliarySpriteParameters
struct PhaseActor {
    u8 pad0[0x104];
    u32 f104;
    u8 pad108[0xc];
    u32 flags114;
    u8 pad118[0x104];
    void (*phase)(void);
    u8 pad220[0x90];
    u8 phaseState;
    u8 pad2b1;
    u8 entered;
    u8 pad2b3[2];
    u8 phaseId;
    u8 pad2b6[0x12];
    u16 timer;
    u8 pad2ca[0x106];
    u8 *data;
};
void Axe_InitAttackTrail(struct PhaseActor *);
void Axe_CopyAuxiliaryVector(struct PhaseActor *, void *);
void ActorShadow_Enable(void *);
void Sound_PlaySong(u32);
u8 ActorPhase_UpdateFlicker(u32 *, s32, s32, s32);
void Axe_SetAuxiliarySpriteParameters(struct PhaseActor *, void *, u32);
void Axe_UpdateOrbitAttack(void);
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
void ActorPhase_UpdateTimedFlicker(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    u32 flags;
    if (consumeEntry(s)) {
        Axe_InitAttackTrail(s);
        Axe_CopyAuxiliaryVector(s, data + 0xc88);
        *(u16 *)(data + 0x90a) = 0;
        *(u32 *)(data + 0xa38) &= ~1;
        ActorShadow_Enable(data + 0xcac);
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
        Sound_PlaySong(0x16f);
    }
    if (ActorPhase_UpdateFlicker(&flags, frame, 10, 30)) {
        void (*next)(void) = Axe_UpdateOrbitAttack;
        u32 id = 0x2c;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
        Sound_PlaySong(0x16d);
    } else if (flags & 1) {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x888, 0x13c);
    } else {
        Axe_SetAuxiliarySpriteParameters(s, data + 0x908, 0x13f);
    }
}
