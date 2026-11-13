#ifndef GUARD_ACTOR_PALETTE_H
#define GUARD_ACTOR_PALETTE_H
#include "armor_palette.h"
#include "render.h"
struct ActorPaletteSource {
    u8 unknown0[0x74];
    u16 banks[3][16];
};
void Palette_CopyBank(void *, const void *);
void *ScriptProc_Start(u16, u16);
void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *, void *, u32, u32);
void Armor_BindPaletteBanks(struct SceneCombatActor *);
void ActorPalette_LoadScriptBanks(struct SceneCombatActor *);
void ActorPalette_RestoreSelectedBanks(struct ArmorPaletteController *);
void ActorPalette_FadeWhiteFast(struct ArmorPaletteController *);
void ActorPalette_FadeWhite(struct ArmorPaletteController *);
void ActorPalette_BlendSavedBanks(struct ArmorPaletteController *);
void ActorPalette_BlendSavedBanksSlow(struct ArmorPaletteController *);
void ActorPalette_PulseBlendSaved(struct ArmorPaletteController *);
void Palette_UpdatePulseFade(struct ArmorPaletteController *);
void Armor_UpdatePaletteRestoreDelay(struct ArmorPaletteController *);
void ActorPalette_ReturnImmediately(struct ArmorPaletteController *);
void Armor_CopyScenePalette(void *);
s32 Callback_InvokeActor(void *, void *);
void Palette_BlendBank(void *, void *, int, int, int);
void Palette_InterpolateBanks(void *, void *, void *, int, int);
#endif
