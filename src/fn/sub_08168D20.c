#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
s32 GameState_GetUnlockEntry(s32);
void Player_BeginIndexedAction(void *, s32);
void Player_BeginDescriptorAction(void *, s32);
void SceneHud_ApplySecondarySelection(struct SceneHudState *p)
{
    s32 offset = p->selection * 8;
    u32 entry = (u32)p + offset;
    u16 field = 0xDAA;
    if (*(u8 *)(entry + field)) {
        void *owner = p->actionOwner;
        field--;
        Player_BeginIndexedAction(owner, GameState_GetUnlockEntry(*(s8 *)(entry + field)));
    } else {
        void *owner = p->actionOwner;
        u32 data = (u32)p + 0xDAC;
        data += offset;
        Player_BeginDescriptorAction(owner, *(s32 *)data);
    }
    {
        u16 *out = &gGameState->unk50;
        struct SceneHudState *selected = (struct SceneHudState *)((u32)p + p->selection * 8);
        *out = selected->secondaryFirstValue;
    }
}
