#include "global.h"
#define ActorPhase_UpdateTimedFlicker sub_080F7868
#define ActorPhase_UpdateFlicker      sub_080FC498
#define ActorPhase_SetAnimation       sub_080FC8B0
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
void sub_080FC4F4(struct PhaseActor *);
void sub_080FC894(struct PhaseActor *, void *);
void ActorShadow_Enable(void *);
void Sound_PlaySong(u32);
u8 sub_080FC498(u32 *, s32, s32, s32);
void sub_080FC8B0(struct PhaseActor *, void *, u32);
void sub_080F79A8(void);
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
void sub_080F7868(struct PhaseActor *s, s32 frame)
{
    u8 *data = s->data;
    u32 flags;
    if (consumeEntry(s)) {
        sub_080FC4F4(s);
        sub_080FC894(s, data + 0xc88);
        *(u16 *)(data + 0x90a) = 0;
        *(u32 *)(data + 0xa38) &= ~1;
        ActorShadow_Enable(data + 0xcac);
        sub_080FC8B0(s, data + 0x888, 0x13c);
        Sound_PlaySong(0x16f);
    }
    if (sub_080FC498(&flags, frame, 10, 30)) {
        void (*next)(void) = sub_080F79A8;
        u32 id = 0x2c;
        s->entered = 1;
        s->phaseState = 0;
        s->phaseId = id;
        s->phase = next;
        s->timer = 0;
        clearFlags(&s->flags114, 1);
        s->f104 = 0;
        sub_080FC8B0(s, data + 0x908, 0x13f);
        Sound_PlaySong(0x16d);
    } else if (flags & 1) {
        sub_080FC8B0(s, data + 0x888, 0x13c);
    } else {
        sub_080FC8B0(s, data + 0x908, 0x13f);
    }
}
