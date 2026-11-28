#include "global.h"
#include "engine/engine.h"
#include "ui/books/palette_transition_banks.h"

struct PaletteTransitionObject {
    u8 unk0[0x3A];
    u16 color;
    u8 unk3C[0x48 - 0x3C];
    void *palette;
};
struct PaletteTransition {
    u8 pad0[0x54];
    struct PaletteTransitionObject object;
    u8 pad_a0[0x3A0 - 0x54 - sizeof(struct PaletteTransitionObject)];
    struct PaletteTransitionBanks banks;
};
extern u32 gUnk_0824FC1C[];
extern u8 gUnk_0824FC50[];
extern u8 gUnk_0824FC5D[];
void PaletteTransition_CopyBanks(struct PaletteTransitionBanks *, u16 *, u32, u32);
// confidence=low: picks the palette source for id (0x64-0x67 remap to 0x36-0x39), fills the banks
// and points the object at the destination palette.
void PaletteTransition_LoadSource(struct PaletteTransition *p, s32 id)
{
    struct PaletteTransitionObject *object = &p->object;
    struct PaletteTransitionBanks *banks = &p->banks;
    u32 resource;
    u16 *src;
    u32 color;
    u32 index;
    u32 bank, extra;
    switch (id) {
        case 0x64:
            id = 0x36;
            break;
        case 0x65:
            id = 0x38;
            break;
        case 0x66:
            id = 0x37;
            break;
        case 0x67:
            id = 0x39;
            break;
    }
    index = id - 0x34;
    resource = gUnk_0824FC1C[index];
    if (resource) {
        src = (u16 *)((u8 *)ScriptProc_Start(0x92B3, resource) + 0x14);
        color = resource | 0xF000;
    } else {
        src = (u16 *)(gPaletteBankData + 0x41E0);
        color = 0xF20F;
    }
    bank = gUnk_0824FC50[index];
    extra = gUnk_0824FC5D[index];
    if (id == 0x3B) {
        banks->firstBank = 6;
        banks->extraBank = 12;
    } else {
        banks->firstBank = bank;
        banks->extraBank = extra;
    }
    PaletteTransition_CopyBanks(banks, src, bank, extra);
    object->color = color;
    object->palette = banks->destination;
}
