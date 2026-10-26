#define SceneActorStyle_CountEntries ModifierMenu_CountAppearanceModifiers
#include "global.h"
#include "game_state.h"
#include "scene_actor_style.h"
s32 ModifierMenu_CountAppearanceModifiers(struct SceneActorStyle *);
void SceneActorStyle_InitializeFromGameState(struct SceneActorStyle *style)
{
    s32 i;
    u32 zero;
    struct GameState *state = gGameState;
    style->adjustedA = *(u8 *)&state->unk7A0[2] + 12;
    style->adjustedB = *(u8 *)&state->unk7A0[0] + 6;
    style->adjustedC = *(u8 *)&state->unk7A0[1] + 18;
    style->parameterE = state->styleParameterE;
    for (i = 0; i <= 3; i++)
        style->entries[i] = ((u16 *)state->modifierIdsBytes)[i];
    zero = 0;
    style->count = ModifierMenu_CountAppearanceModifiers(style);
    state = gGameState;
    style->parameterD = (u16)state->unk81A;
    style->parameterB = state->unk7A0[0];
    style->parameterF = state->unk7A0[3];
    style->parameterC = state->unk7A0[1];
    style->parameterA = state->unk7A0[2];
    {
        u16 initial;
        state = (void *)((u8 *)state + 0x818);
        initial = *(u16 *)state;
        style->initialHalfword = initial;
        style->currentHalfword = initial;
    }
    style->initialWord = zero;
    style->currentWord = zero;
}
