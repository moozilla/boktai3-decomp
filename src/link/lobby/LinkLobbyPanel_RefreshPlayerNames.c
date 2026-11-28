#include "global.h"
#include "link/lobby/record_menu.h"
#include "link/records/link_record_panel.h"
#include "scene/areas/area_effect_link.h"

void LinkLobbyPanel_RefreshPlayerNames(struct RecordMenu *menu)
{
    struct LinkTimeRecord *peer = getPeer(0);

    if (peer) {
        TextWindow_SetScript(menu->nameWindows[0], menu->recordScript);
        TextWindow_Refresh(menu->nameWindows[0], 0);
        TextWindow_SetString(menu->nameWindows[0], 1, peer->displayName);
        TextWindow_StartPlayback(menu->nameWindows[0]);
        peer = getPeer(1);
        if (peer) {
            TextWindow_SetScript(menu->nameWindows[1], menu->recordScript);
            TextWindow_Refresh(menu->nameWindows[1], 1);
            TextWindow_SetString(menu->nameWindows[1], 2, peer->displayName);
            TextWindow_StartPlayback(menu->nameWindows[1]);
        }
    }
}
