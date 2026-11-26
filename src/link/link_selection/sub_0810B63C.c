#include "global.h"
#include "link/link_selection/link_selection_mode.h"
#define SceneMenu_SetState Registry1745_SetState
struct SceneMenu {
    u8 pad0[0x1A];
    u16 count;
    u8 pad1C[0x14];
    u32 timer;
    u8 pad34[0xA];
    u16 flags;
    u16 *input;
    u32 field44;
    u32 state;
};
u8 Registry1745_ConsumeStateEntry(struct SceneMenu *);
void Registry1745_BeginShutdown(struct SceneMenu *, void (*)(void));
void Registry1745_SetState(struct SceneMenu *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Registry1745_WaitForCompletionFlag(void);
static inline u16 ReadInput(struct SceneMenu *p, s32 i)
{
    if (i < 0)
        return 0xFFFF;
    return p->input[i];
}
s32 SceneMenu_WaitForConfirmation(struct SceneMenu *p)
{
    s32 completed, i;
    Registry1745_ConsumeStateEntry(p);
    completed = 0;
    for (i = 0; i < p->count; i++) {
        u16 v = ReadInput(p, i);
        if (LinkSelection_IsReady(v, 0x3c00) && (v & 0x3F) == 2)
            completed++;
    }
    if (completed == p->count)
        Registry1745_SetState(p, 0x1B, Registry1745_WaitForCompletionFlag);
    else if (p->state + 2 <= 1 || p->timer > 179) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    p->flags = 0x1002;
}
