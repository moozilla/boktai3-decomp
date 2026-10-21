// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "modifier_list.h"

#ifdef NONMATCHING
// fuzzy: 97.6%
// foundry draft (worker best 88.0%); what differs is mostly register allocation and instruction order
#define CharacterPreview_SelectResource CharacterModel_GetPrimaryPaletteIndex
#define gCharacterPaletteResource       gUnk_030042E4
struct CharacterPreviewTileAllocator {
    u8 unknown0[0x38];
    s32 nextTile;
};
extern struct CharacterPreviewTileAllocator *gUnk_020004B8;
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
void CharacterPreview_InitializePalette(struct CharacterPreviewPalette *p,
                                        struct CharacterPreviewModel *model, s32 index)
{
    u16 *palette;
    s32 resource;
    s32 offset, i;
    register struct CharacterPreviewPalette *flash asm("r5") = p;
    flash->target = model;
    flash->flashPalette = gUnk_030042E4 + 0x3240;
    switch (model->view.metadata.kind) {
        case 0:
            resource = CharacterModel_GetPrimaryPaletteIndex(model);
            break;
        case 1:
            resource = 0x106;
            break;
        case 2:
            resource = 0x107;
            break;
        case 3:
            resource = 0x108;
            break;
        case 4:
        case 5:
            resource = 0x109;
            break;
        default:
            resource = 0x378;
            break;
    }
    flash->paletteId = index + 0x379;
    {
        const u8 *source = gUnk_030042E4 + (resource << 5);
        flash->normalPalette = gUnk_030042E4 + (flash->paletteId << 5);
        palette = model->activePalette;
        CpuSet(source, palette, 16);
    }
    CpuSet(flash->normalPalette, model->normalPalette, 16);
    offset = 0;
    i = 4;
    do {
        struct ModifierDisplayObject *part = (void *)((u8 *)flash->target + offset);
        part->palette = palette;
        part->tile = gUnk_020004B8->nextTile;
        offset += 0x60;
    } while (--i >= 0);
    gUnk_020004B8->nextTile += 2;
    if (gUnk_020004B8->nextTile > 0xafff)
        gUnk_020004B8->nextTile = 0xa000;
    else if (gUnk_020004B8->nextTile <= 0x9fff)
        gUnk_020004B8->nextTile = 0xa000;
}
#else
INCLUDE_ASM("asm/nonmatching", CharacterPreview_InitializePalette);
#endif
