#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_ShowReadyMessage  sub_0810A7E8
#define LinkMenu_SetState Registry1745_SetState
#define LinkMenu_CheckSession Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection Registry1745_BeginShutdown
#define gLinkMenuState             gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void sub_08109C80(void);
void sub_0810A8D8(void);
void Hud_RefreshSpriteFromScript(void);
void Hud_SetMode(s32);
s32 sub_08033690(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
void sub_0810BE0C(void *, void *);
static inline u16 SelectionAt(struct AreaEffectLinkState *p, s32 index)
{
    if (index < 0)
        return 0xffff;
    return p->selection[index];
}
static inline u8 IsChoiceMode(u32 value, u32 mask)
{
    value &= mask;
    if (value == 0x1000)
        return 1;
    return 0;
}
extern u32 gUnk_03004DA0, gUnk_030051D4;
s32 sub_0810A7E8(struct AreaEffectLinkState *p)
{
    s32 count, i, limit;
    u32 mask;
    if (Registry1745_ConsumeStateEntry(p)) {
        s32 *window;
        gUnk_03004DA0 = 0x40;
        gUnk_030051D4 = 0x40;
        Hud_RefreshSpriteFromScript();
        Hud_SetMode(5);
        p->menuTextWindow = sub_08033690(1, 6, 28, 6);
        window = &p->menuTextWindow;
        TextWindow_SetScript(*window, p->menuMessageScript);
        TextWindow_Refresh(*window, 8);
        TextWindow_StartPlayback(*window);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    count = 0;
    i = 0;
    limit = p->participantCount;
    if (i < limit) {
        mask = 0x3c00;
        do {
            u16 value = SelectionAt(p, i);
            if (IsChoiceMode(value, mask) && (value & 0xff) == 0xd)
                count++;
        } while (++i < limit);
    }
    if (count == limit)
        Registry1745_SetState(p, 13, sub_0810A8D8);
    else if (p->menuElapsed > 60)
        p->requestedMode = 0x100d;
    return 0;
}
