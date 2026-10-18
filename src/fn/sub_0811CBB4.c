#include "global.h"
#include "game_state.h"
#define ScenePalette_CopySelectedColors CharacterModel_LoadPrimaryPalette
#define gGameState                      gUnk_02000710
extern u32 gUnk_030053F4, gUnk_030053F0;
void CharacterModel_LoadPrimaryPalette(u8 *);
struct ScenePaletteControl {
    u8 pad[0x4b];
    u8 state;
    u8 paletteMode;
};
static inline u8 HasRefreshFlag(void)
{
    u32 mask = 0x100;
    if (gUnk_02000710->flags868 & mask)
        return TRUE;
    return FALSE;
}
static inline u32 TestInput(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
void ScenePalette_RefreshOnFlags(struct ScenePaletteControl *s, u8 *palette)
{
    if (HasRefreshFlag() || TestInput(0x800)) {
        if (s->state == 11)
            s->paletteMode = 3;
        else
            s->paletteMode = 0;
        CharacterModel_LoadPrimaryPalette(palette);
    }
}
