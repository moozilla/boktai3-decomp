#include "global.h"
#include "sun_clock.h"
#include "game_state.h"

// foundry draft (worker best 98.1%); what differs is mostly register allocation and instruction order
#define gSunClock gUnk_03005430
struct ClockPanelSnapshot {
    u8 bytes[0x20];
    u16 time;
    u16 party;
    s32 values[5];
    u8 unk38[0x88];
    u8 record[1];
};
struct ClockPanel {
    u8 unk0[0x1D];
    u8 mode;
    u8 unk1E[0xC2];
    struct ClockPanelSnapshot snapshot;
};
s32 GameState_GetChallengeBestScore(u32);
u32 GameState_HasEquippedConsumable(s32);
void SceneActorStyle_InitializeFromGameState(void *);
void ClockPanel_CaptureSnapshot(struct ClockPanel *p)
{
    u8 *record;
    struct ClockPanelSnapshot *s = &p->snapshot;
    u8 *dest;
    s32 *values;
    u8 *src;
    s32 i;
    s32 party;
    unsigned int new_var;
    new_var = 16;
    record = s->bytes;
    src = (u8 *)gGameState + 0x780;
    for (i = 0; i < 16; i++)
        s->bytes[i] = src[i];
    src = (u8 *)gGameState + 0x790;
    i = 0;
    values = s->values;
    record += 0xC0;
    dest = s->bytes + new_var;
    for (; i < 16; i++)
        dest[i] = src[i];
    for (i = 0; i < 5; i++)
        *values++ = GameState_GetChallengeBestScore(i + 6);
    if (!p->mode)
        s->time = gUnk_03005430.extra6 * 15 + gUnk_03005430.extra7;
    else
        s->time = 0;
    i = 0;
    if (GameState_HasEquippedConsumable(0xA)) {
        party = 2;
        i = 1;
    }
    if (GameState_HasEquippedConsumable(0x1A)) {
        party = 3;
        i++;
    }
    if (GameState_HasEquippedConsumable(0x25)) {
        party = 4;
        i++;
    }
    if (GameState_HasEquippedConsumable(0x2F)) {
        party = 5;
        i++;
    }
    {
        u32 value = 1;
        if (i <= 3) {
            value = 0;
            if (i == 1)
                value = party;
        }
        s->party = value;
    }
    SceneActorStyle_InitializeFromGameState(record);
}
