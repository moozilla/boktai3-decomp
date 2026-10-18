#define SceneTask_UpdateTextDelay sub_0810F2DC
#include "global.h"
void Task_Kill(void *);
s32 TextWindow_Refresh(s32, s32);
void Registry1745_SetCancelRequest(u32);
s32 Registry1745_GetSessionState(void);
void LinkLobbyPanel_HideSprites(void *);
struct SceneTextTask {
    u8 unknown0[0x14F0];
    s32 window;
    u8 unknown14F4[8];
    u8 reset;
    u8 unknown14FD;
    u16 timer;
};
void sub_0810F2DC(struct SceneTextTask *s)
{
    u32 state;
    if (s->reset) {
        Registry1745_SetCancelRequest(1);
        s->timer = 0;
        s->reset = 0;
    }
    if (s->timer <= 9) {
        s->timer++;
    } else if (s->timer == 10) {
        TextWindow_Refresh(s->window, 8);
        s->timer++;
    }
    state = Registry1745_GetSessionState();
    if (state == 0x15 || state == 0x17) {
        LinkLobbyPanel_HideSprites(s);
        Task_Kill(s);
    }
}
