#include "global.h"
#define SceneMenu_SetState Registry1745_SetState
#define SceneMenu_WaitForConfirmation sub_0810B63C
struct SceneMenu {
    u8 pad0[0x1A]; volatile u16 count;
    u8 pad1C[0x14]; u32 timer;
    u8 pad34[0xA]; u16 flags;
    u16 *input;
    u32 field44;
    u32 state;
};
u8 Registry1745_ConsumeStateEntry(struct SceneMenu *);
void Registry1745_BeginShutdown(struct SceneMenu *, void (*)(void));
void Registry1745_SetState(struct SceneMenu *, u32, void (*)(void));
void sub_08109C80(void);
void Registry1745_WaitForCompletionFlag(void);
static inline u16 ReadInput(struct SceneMenu *p, s32 i)
{
    if (i < 0) return 0xFFFF;
    return p->input[i];
}
static inline u8 IsAcknowledgement(u32 v, u32 mask)
{
    if ((v & mask) == 0x1000) return 1;
    return 0;
}
s32 sub_0810B63C(struct SceneMenu *p)
{
    s32 completed, i;
    u32 mask;
    Registry1745_ConsumeStateEntry(p);
    completed = 0;
    i = 0;
    if (completed < p->count) {
        mask = 0x3C00;
        do {
            u16 v = ReadInput(p, i);
            if (IsAcknowledgement(v, mask) && (v & 0x3F) == 2)
                completed++;
            i++;
        } while (i < p->count);
    }
    if (completed == p->count)
        Registry1745_SetState(p, 0x1B, Registry1745_WaitForCompletionFlag);
    else if (p->state + 2 <= 1 || p->timer > 179) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    p->flags = 0x1002;
}
