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
        struct GameState *state = gGameState;
        s16 *row = (s16 *)((u8 *)state + i * sizeof(s16));
        s32 id = ((s16 *)((struct GameState *)row)->modifierIdsBytes)[0];
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
