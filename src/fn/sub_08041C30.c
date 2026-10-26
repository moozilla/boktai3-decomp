// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
void Modifier_LoadFirstSource(struct ModifierSelection *);
void Modifier_LoadSecondSource(struct ModifierSelection *, const struct ModifierSelection *);
void Modifier_LoadThirdSource(struct ModifierSelection *);
void Modifier_LoadFourthSource(struct ModifierSelection *);
void Modifier_AddSelection(struct ModifierSelection *, u32);
void ModifierMenu_CommitFirstSource(struct ModifierMenu *);
void ModifierMenu_CommitSecondSource(struct ModifierMenu *);
void ModifierMenu_CommitThirdSource(struct ModifierMenu *);
void ModifierMenu_CommitFourthSource(struct ModifierMenu *);
void ModifierMenu_CommitSelection(struct ModifierMenu *);
void ModifierMenu_ReconcileSelection(struct ModifierMenu *);
void ModifierMenu_CommitSecondarySource(struct ModifierMenu *);
void ModifierMenu_LoadSavedSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    s32 i;
    pending->sourceIds[0] = gGameState->unk7A0[0];
    Modifier_LoadFirstSource(pending);
    pending->sourceIds[1] = gGameState->unk7A0[2];
    Modifier_LoadSecondSource(pending, &menu->current);
    pending->sourceIds[2] = gGameState->unk7A0[1];
    Modifier_LoadThirdSource(pending);
    pending->sourceIds[3] = gGameState->unk7A0[3];
    Modifier_LoadFourthSource(pending);
    pending->count = 0;
    for (i = 0; i < 4; i++) {
        register struct GameState *state asm("r0") = gGameState;
        s16 *row;
        register s32 id asm("r0");
        row = (s16 *)(i * 2 + (u32)state);
        id = *(s16 *)((u8 *)row + (u32) & ((struct GameState *)0)->modifierIdsBytes);
        if (id >= 0) {
            s32 argument = id;
            Modifier_AddSelection(pending, argument);
        }
    }
    pending->selectedId = ((s16 *)gGameState->modifierIdsBytes)[0];
    if (pending->selectedId < 0)
        pending->selectedId = 0;
    pending->secondaryId = gGameState->unk81A;
    ModifierMenu_CommitFirstSource(menu);
    ModifierMenu_CommitSecondSource(menu);
    ModifierMenu_CommitThirdSource(menu);
    ModifierMenu_CommitFourthSource(menu);
    ModifierMenu_CommitSelection(menu);
    ModifierMenu_ReconcileSelection(menu);
    ModifierMenu_CommitSecondarySource(menu);
}
