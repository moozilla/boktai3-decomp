#include "global.h"
#include "link/records/link_record_panel.h"
#include "ui/save/save_menu.h"
void TextWindow_SetString(u32, u32, void *);
void TextWindow_StartPlayback(u32);
static inline u8 ok(s32 v)
{
    if (v < 0)
        goto z;
    if (v <= 1)
        goto o;
z:
    return 0;
o:
    return 1;
}
void LinkScorePanel_RefreshPlayerName(struct SaveMenu *menu)
{
    s32 i = menu->localPlayer;
    struct AreaEffectLinkState *link = gLinkSession;
    if (link != NULL && link->unavailable == 1 && ok(i)) {
        struct LinkTimeRecord *record = &((struct LinkRecordPayload *)link->sessionData)->peers[i];
        if (record != NULL) {
            TextWindow_SetScript(menu->nameWindow, menu->nameScript);
            TextWindow_Refresh(menu->nameWindow, 0);
            TextWindow_SetString(menu->nameWindow, 1, record->displayName);
            TextWindow_StartPlayback(menu->nameWindow);
        }
    }
}
