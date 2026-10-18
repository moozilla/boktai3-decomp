#define InventoryGrid_SetPhaseCallback Hud_SetInteractionCallback
#define InventoryGrid_BeginPaletteTransition PauseMenu_StartPaletteTransition
#define InventoryGrid_UpdateIconMotion PauseMenu_UpdateSpriteMotion
#include "global.h"
struct InventoryGrid {
    u8 unknown0000[0xA4E];
    u16 phaseCounter;
    u8 unknown0A50[0x2AA8 - 0xA50];
    u32 flags2AA8;
    u8 unknown2AAC[0x438A - 0x2AAC];
    u8 selector438A;
    u8 unknown438B[0x4866 - 0x438B];
    s16 value4866;
};
u32 PauseMenu_UpdateSpriteMotion(void *, u32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
u32 PlayerPanel_GetStatusValue(void *);
void PauseMenu_StartPaletteTransition(void *, u32, u32, s32);
void sub_0816D634(void *, u32);
void InventoryGrid_ShowNumber(void *, s32, s16 *);
void InventoryGrid_UpdateNumberPhase(void);
void Sound_PlaySong(u32);
struct InventoryGridVector {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
static inline void SetVector(struct InventoryGridVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void InventoryGrid_BeginNumberPhase(struct InventoryGrid *s)
{
    struct InventoryGridVector vec;
    if (PauseMenu_UpdateSpriteMotion(s, 0)) {
        if (++s->phaseCounter > 7) {
            s->flags2AA8 |= 1;
            sub_0816D634(s, s->selector438A);
            PauseMenu_StartPaletteTransition(s, 0x16E, PlayerPanel_GetStatusValue(s), 16);
            SetVector(&vec, 32, 48, 0);
            InventoryGrid_ShowNumber(s, s->value4866, (s16 *)&vec);
            Sound_PlaySong(0x10D);
            Hud_SetInteractionCallback(s, InventoryGrid_UpdateNumberPhase, 1);
        }
    }
}
