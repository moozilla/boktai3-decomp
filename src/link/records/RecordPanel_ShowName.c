#include "engine/engine_pause.h"
#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "scene/areas/area_effect_link.h"
#include "link/records/record_name_panel.h"
#define TextWindow_Show TextWindow_StartPlayback
struct PanelNameRecord {
    u8 pad0[0x10];
    u8 name[16];
};
struct PanelSelection {
    u8 pad0[0x53];
    u8 type;
    u8 pad54[0xCC];
    struct PanelNameRecord record;
};
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
static inline struct PanelNameRecord *GetNameRecord(void)
{
    struct PanelSelection *p = gLinkSession;
    if (!p)
        return 0;
    if (p->type != 1)
        return 0;
    return &p->record;
}
void RecordPanel_ShowName(struct RecordNamePanel *p)
{
    u8 *name;
    if (Engine_PauseBits(0x800)) {
        struct PanelNameRecord *record = GetNameRecord();
        if (!record)
            return;
        name = record->name;
    } else {
        name = &gGameState->playerName[16];
    }
    TextWindow_SetScript(p->win[0], p->nameScript);
    TextWindow_Refresh(p->win[0], 0);
    TextWindow_SetString(p->win[0], 1, name);
    TextWindow_StartPlayback(p->win[0]);
}
