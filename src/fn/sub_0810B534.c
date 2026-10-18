#include "global.h"
#define SceneMenu_SetState Registry1745_SetState
struct SceneMenu {
    u8 pad0[0x1A]; u16 count;
    u8 pad1C[0xB]; u8 timer;
    u8 pad28; u8 active;
    u8 pad2A[0x14]; u16 flags;
    u16 *input;
    u8 pad44[0x80]; u32 script;
};
u8 Registry1745_ConsumeStateEntry(struct SceneMenu *);
s32 Registry1745_CheckSessionReady(struct SceneMenu *);
void Registry1745_BeginShutdown(struct SceneMenu *, void (*)(void));
void Registry1745_SetState(struct SceneMenu *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Registry1745_WaitForReadyFlag(void);
void Script_Call(u32, void *);
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
s32 SceneMenu_WaitForAcknowledgements(struct SceneMenu *p)
{
    s32 completed, i, count;
    u32 mask;
    if (Registry1745_ConsumeStateEntry(p)) {
        if (p->script) Script_Call(p->script, 0);
        p->timer = 0;
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    completed = 0;
    i = 0;
    count = p->count;
    if (completed < count) {
    mask = 0x3C00;
    do {
        u16 v = ReadInput(p, i);
        if (IsAcknowledgement(v, mask) && (v & 0x3F) == 1)
            completed++;
        i++;
    } while (i < count);
    }
    if (completed == count)
        Registry1745_SetState(p, 0x19, Registry1745_WaitForReadyFlag);
    if (p->active) {
        if (p->timer > 59) p->flags = 0x1001;
        else p->timer++;
    }
}
