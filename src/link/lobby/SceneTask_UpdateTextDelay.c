#include "global.h"
#include "engine/engine.h"
#include "link/lobby/record_menu.h"

void LinkSession_SetCancelRequest(u32);
s32 LinkSession_GetSessionState(void);
void LinkLobbyPanel_HideSprites(struct RecordMenu *menu);

void SceneTask_UpdateTextDelay(struct RecordMenu *menu)
{
    u32 state;

    if (menu->redraw) {
        LinkSession_SetCancelRequest(1);
        menu->inputDelay = 0;
        menu->redraw = 0;
    }
    if (menu->inputDelay <= 9) {
        menu->inputDelay++;
    } else if (menu->inputDelay == 10) {
        TextWindow_Refresh(menu->nameWindows[0], 8);
        menu->inputDelay++;
    }
    state = LinkSession_GetSessionState();
    if (state == 0x15 || state == 0x17) {
        LinkLobbyPanel_HideSprites(menu);
        Task_Kill(menu);
    }
}
