#include "global.h"
#include "system/clock/sun_clock.h"
#include "system/game_state/game_state.h"
#include "scene/actor/scene_actor_style.h"
#include "scene/areas/area_effect_link.h"

struct ClockPanelSnapshot {
    u8 nameBytes[32];
    u16 time;
    u16 party;
    s32 values[5];
    u8 unk38[0x88];
    struct SceneActorStyle style;
};
s32 GameState_GetChallengeBestScore(u32);
u32 GameState_HasEquippedConsumable(s32);
void SceneActorStyle_InitializeFromGameState(struct SceneActorStyle *);
void ClockPanel_CaptureSnapshot(struct AreaEffectLinkState *p)
{
    u8 *style;
    struct ClockPanelSnapshot *s = (struct ClockPanelSnapshot *)p->sessionData;
    u8 *dest;
    s32 *values;
    u8 *src;
    s32 i;
    s32 party;
    unsigned int bikeNameOffset;
    bikeNameOffset = 16;
    style = (u8 *)s;
    src = gGameState->playerName;
    for (i = 0; i < 16; i++)
        s->nameBytes[i] = src[i];
    src = gGameState->playerName + 16;
    i = 0;
    values = s->values;
    style += (u8 *)&s->style - (u8 *)s;
    dest = s->nameBytes + bikeNameOffset;
    for (; i < 16; i++)
        dest[i] = src[i];
    for (i = 0; i < 5; i++)
        *values++ = GameState_GetChallengeBestScore(i + 6);
    if (!p->exchangeBusy)
        s->time = gSunClock.extra6 * 15 + gSunClock.extra7;
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
    SceneActorStyle_InitializeFromGameState((struct SceneActorStyle *)style);
}
