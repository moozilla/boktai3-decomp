#define gLinkMenuFlags                         gUnk_0300523C
#define gLinkMenuActive                        gUnk_03005250
#include "global.h"
#include "area_effect_link.h"
#include "game_state.h"
u8 Registry1745_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 Registry1745_CheckSessionReady(struct AreaEffectLinkState *);
void Registry1745_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Registry1745_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void sub_0810B238(void);
void sub_0810BBD4(struct AreaEffectLinkState *);
void PaletteLevels_Reset(void);
void Sound_FadeOutChannel10(u32);
extern u32 gUnk_0300523C, gUnk_03005250;
static inline u16 GetSlot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 IsReadySlot(u32 value, u32 mask)
{
    if ((value & mask) == 0x1000)
        return 1;
    return 0;
}
/* The waiting path preserves r0 from the validity check; no explicit return in the original. */
s32 LinkSelection_WaitForReadyParticipants(struct AreaEffectLinkState *p)
{
    struct AreaEffectLinkState *session = p;
    s32 count, i, length, result;
    if (Registry1745_ConsumeStateEntry(p)) {
        u32 mask = 4;
        s16 *counter;
        u16 value;
        s32 increment;
        gUnk_0300523C |= mask;
        PaletteLevels_Reset();
        Sound_FadeOutChannel10(6);
        gUnk_03005250 = 0;
        counter = &gUnk_02000710->unk7B4;
        value = *counter;
        if (*counter < 0)
            value = 0;
        *counter = value;
        counter = &gUnk_02000710->unk7B4;
        increment = *counter + 1;
        if (increment > 9999)
            increment = 9999;
        *counter = increment;
    }
    result = Registry1745_CheckSessionReady(p);
    if (result < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    i = 0;
    length = p->participantCount;
    if (i < length) {
        u32 mask = 0x3c00;
        do {
            u16 slot = GetSlot(p, i);
            if (IsReadySlot(slot, mask) && (slot & 0x3f) == 0)
                count++;
            i++;
        } while (i < length);
    }
    if (count == length) {
        Registry1745_SetState(p, 20, sub_0810B238);
        p->readyPhase = 0;
        sub_0810BBD4(session);
        return 0;
    }
    if (p->menuElapsed > 89)
        p->requestedMode = 0x1000;
}
