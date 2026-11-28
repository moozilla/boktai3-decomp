#include "global.h"
#include "engine/engine.h"
#include "link/lobby/record_menu.h"

s32 LinkSession_GetSessionState(void *);
void ScrollingDialogue_HideImmediately(void);

s32 LinkLobbyPanel_OnTaskUpdate(struct RecordMenu *menu)
{
    if (LinkSession_GetSessionState(menu) == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(menu);
        return -1;
    }
    if (menu->update)
        menu->update(menu);
    return 1;
}
