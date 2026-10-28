#include "global.h"
#include "modifier_list.h"
#include "background_controller.h"

// foundry draft (worker best 88.0%); what differs is mostly register allocation and instruction order
#define CharacterPreview_SelectResource CharacterModel_GetPrimaryPaletteIndex
#define gCharacterPaletteResource       gPaletteBankData
struct CharacterPreviewTileAllocator {
    u8 unknown0[0x38];
    s32 nextTile;
};
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
void CharacterPreview_InitializePalette(struct CharacterPreviewPalette *p,
                                        struct CharacterPreviewModel *model, s32 index)
{
    u16 *palette;
    s32 resource;
    s32 offset, i;
    struct CharacterPreviewPalette *flash = p;
    p->target = model;
    flash->flashPalette = gPaletteBankData + 0x3240;
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
    p->paletteId = index + 0x379;
    {
        const u8 *source = gPaletteBankData + (resource << 5);
        p->normalPalette = gPaletteBankData + (p->paletteId << 5);
        palette = model->activePalette;
        CpuSet(source, palette, 16);
    }
    CpuSet(p->normalPalette, model->normalPalette, 16);
    {
        struct CharacterPreviewTileAllocator **ap = &gSceneCamera;
        struct CharacterPreviewTileAllocator **lp;
        lp = ap;
        offset = 0;
        i = 4;
        do {
            struct ModifierDisplayObject *part = (void *)((u8 *)p->target + offset);
            part->palette = model->activePalette;
            part->tile = (*lp)->nextTile;
            offset += 0x60;
        } while (--i >= 0);
        (*ap)->nextTile = (*ap)->nextTile + (offset = 2);
        if ((*ap)->nextTile > 0xafff) {
            (*ap)->nextTile = 0xa000;
        } else if ((*ap)->nextTile <= 0x9fff) {
            (*ap)->nextTile = 0xa000;
        }
    }
}
