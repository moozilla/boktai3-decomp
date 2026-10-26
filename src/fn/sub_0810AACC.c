#include "global.h"
#include "render.h"

struct SceneSelection {
    u8 pad0[0x1a];
    u16 slotCount;
    u8 pad1c[0x14];
    u32 timer;
    u8 pad34[10];
    u16 command;
    u16 *slots;
};
extern u32 gUnk_03005250;
u8 Registry1745_ConsumeStateEntry(struct SceneSelection *);
s32 Registry1745_CheckSessionReady(struct SceneSelection *);
void Registry1745_BeginShutdown(struct SceneSelection *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Sound_FadeOutChannel10(u32);
void Registry1745_SetState(struct SceneSelection *, u32, void (*)(void));
void SessionMenu_CreateStageIconDisplay(void);
static inline u16 GetSlot(struct SceneSelection *p, s32 i)
{
    if (i < 0)
        return 0xffff;
    return p->slots[i];
}
static inline u8 IsTransitionSlot(u32 value)
{
    if (value == 0x2000)
        return 1;
    return 0;
}
s32 SceneSelection_WaitForSlotTransition(struct SceneSelection *p)
{
    s32 count, i, length;
    gUnk_03005250 = 1;
    if (Registry1745_ConsumeStateEntry(p)) {
        gBgFadeLevel = 0x40;
        gObjFadeLevel = 0x40;
        Sound_FadeOutChannel10(1);
    }
    if (p->timer == 1) {
        u32 mask = 4;
        gEnginePauseFlags |= mask;
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    i = 0;
    length = p->slotCount;
    if (i < length) {
        unsigned short mask = 0x3c00;
        if (1) {
            do {
                if (IsTransitionSlot(GetSlot(p, i) & mask))
                    count++;
                i++;
            } while (i < length);
        }
    }
    if (count == length)
        Registry1745_SetState(p, 16, SessionMenu_CreateStageIconDisplay);
    else if (p->timer > 9) {
        p->command = 0x2000;
        count = 4;
        {
            u32 *flags = &gEnginePauseFlags;
            u32 mask = ~count;
            *flags &= mask;
        }
    }
    return 0;
}
