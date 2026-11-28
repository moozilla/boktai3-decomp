#include "engine/engine_pause.h"
#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "scene/areas/area_effect_link.h"
#include "link/records/link_record_panel.h"
#include "ui/widgets/selection_panel.h"
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
void RecordPanel_ShowName(struct SelectionPanel *panel)
{
    u8 *name;
    if (Engine_PauseBits(0x800)) {
        struct LinkTimeRecord *record = getPeer(0);
        if (!record)
            return;
        name = record->displayName;
    } else {
        name = &gGameState->playerName[16];
    }
    TextWindow_SetScript(panel->windows[0], panel->nameScript);
    TextWindow_Refresh(panel->windows[0], 0);
    TextWindow_SetString(panel->windows[0], 1, name);
    TextWindow_StartPlayback(panel->windows[0]);
}
