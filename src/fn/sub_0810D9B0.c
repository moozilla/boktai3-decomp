#include "global.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"
#define TextWindow_Show TextWindow_StartPlayback
struct RecordPanel {
    u8 pad0[0xCF4];
    const u8 *nameScript;
    u8 padCF8[0x10];
    s32 windows[7];
};
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
extern struct PanelSelection *gUnk_020004B4;
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
static inline struct PanelNameRecord *GetNameRecord(void)
{
    struct PanelSelection *p = gUnk_020004B4;
    if (!p)
        return 0;
    if (p->type != 1)
        return 0;
    return &p->record;
}
void RecordPanel_ShowName(struct RecordPanel *p)
{
    u8 *name;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        struct PanelNameRecord *record = GetNameRecord();
        if (!record)
            return;
        name = record->name;
    } else {
        name = &gGameState->playerName[16];
    }
    TextWindow_SetScript(p->windows[0], p->nameScript);
    TextWindow_Refresh(p->windows[0], 0);
    TextWindow_SetString(p->windows[0], 1, name);
    TextWindow_StartPlayback(p->windows[0]);
}
