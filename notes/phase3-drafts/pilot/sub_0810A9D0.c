// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "engine/ram.h"

#define gLinkSelectionDisplaySlots gSceneSelectionDisplaySlots
u8 LinkSession_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 LinkSession_CheckSessionReady(struct AreaEffectLinkState *);
void LinkSession_SetMenuRequest(u32);
void Script_Call(u32, void *);
void LinkSession_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Bg_ClearTilemap(s32);
void ScriptNotifier_CallWithValue(struct AreaEffectLinkState *);
void LinkSession_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void SceneSelection_WaitForSlotTransition(void);
static inline u16 GetSlot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 IsItemSlot(u32 value, u32 mask)
{
    if ((value & mask) == 0x1000)
        return 1;
    return 0;
}
s32 LinkSelection_WaitForEmptySlots(struct AreaEffectLinkState *p)
{
    u8 *extra = (u8 *)p + 0xe0;
    s32 i;
    s32 count;
    s32 length;
    u32 mask;
    if (LinkSession_ConsumeStateEntry(p)) {
        LinkSession_SetMenuRequest(0);
        Script_Call(p->emptySelectionScript, 0);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    i = 0;
    length = p->participantCount;
    while (i < length) {
        u16 slot = GetSlot(p, i);
        if (IsItemSlot(slot, 0x3c00) && (slot & 0xff) == 0x23)
            count++;
        i++;
    }
    if (count == length) {
        u16 *q;
        u16 *base;
        Bg_ClearTilemap(0);
        p->menuSelection = extra[0x130];
        ScriptNotifier_CallWithValue(p);
        base = gSceneSelectionDisplaySlots;
        {
            u32 fill = 0x3ff;
            q = base + 3;
            do {
                *q = fill;
                q--;
            } while ((s32)q >= (s32)base);
        }
        LinkSession_SetState(p, 15, SceneSelection_WaitForSlotTransition);
    } else if (p->menuEvent == 1)
        p->requestedMode = 0x1023;
    return 0;
}
