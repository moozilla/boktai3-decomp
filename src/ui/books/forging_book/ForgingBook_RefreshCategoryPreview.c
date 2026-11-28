#include "global.h"
#include "ui/books/forging_book.h"
#include "engine/video/render.h"

void VariantSprite_SelectAnimation(void *, u32);
void PlayerAttackEffect_SetEmissionState(u32);

s32 ForgingBook_RefreshCategoryPreview(struct ForgingBook *book)
{
    book->animationMode = 2;
    CpuSet(gPaletteBankData + 0x540, book->previewColors, 0x10);
    switch (book->category) {
    case 0:
        book->previewStep = 0;
        VariantSprite_SelectAnimation(book->debrisTask, 1);
        book->animationIndex = 0x5e;
        break;
    case 1:
        VariantSprite_SelectAnimation(book->debrisTask, 3);
        book->animationIndex = 0x54;
        break;
    case 2:
        VariantSprite_SelectAnimation(book->debrisTask, 4);
        book->animationIndex = 0x54;
        break;
    case 3:
        VariantSprite_SelectAnimation(book->debrisTask, 0);
        book->animationIndex = 0x54;
        break;
    case 4:
        VariantSprite_SelectAnimation(book->debrisTask, 2);
        book->animationIndex = 0x81;
        break;
    case 5: {
        s32 discovered = ForgingBook_IsWeaponDiscovered(0x30);

        if (discovered) {
            book->animationIndex = 0x1b6;
        } else {
            book->animationBase = 3;
            book->animationFlag = 1;
            book->animationIndex = discovered;
            Anim_LoadKeyframe(&book->preview, &book->previewResource, book->animationBase, 2);
            Anim_SetKeyframes(&book->preview, &book->previewResource,
                              (u16)(book->animationIndex + book->animationBase), 2, 4);
            book->frame = discovered;
            return 0;
        }
        break;
    }
    case 6:
        book->animationIndex = 0x1bc;
        book->animationMode = 1;
        break;
    case 7:
        book->animationIndex = 0x89;
        book->animationMode = 2;
        break;
    case 8:
        book->animationIndex = 0x39;
        book->animationMode = 2;
        break;
    case 9:
        book->animationIndex = 8;
        book->animationMode = 1;
        break;
    }

    if (book->category <= 5) {
        book->animationBase = 3;
        book->animationFlag = 1;
        Anim_LoadKeyframe(&book->preview, &book->previewResource,
                          (u16)(book->animationIndex + book->animationBase), 2);
        Anim_SetKeyframes(&book->preview, &book->previewResource,
                          (u16)(book->animationIndex + book->animationBase), 2, 4);
        if (book->category <= 4) {
            PlayerAttackEffect_SetEmissionState(2);
            PlayerAttackEffect_SetEmissionState(1);
            if (book->category == 0)
                PlayerAttackEffect_SetEmissionState(3);
            if (book->category == 4)
                PlayerAttackEffect_SetEmissionState(2);
        } else {
            const u16 *colors = (const u16 *)(gPaletteBankData + 0x6a0);

            book->previewColors[5] = colors[12];
            book->previewColors[6] = colors[13];
            book->previewColors[13] = colors[14];
        }
    } else {
        book->animationBase = 0;
        if (book->category == 8)
            book->animationFlag = 0;
        else
            book->animationFlag = 1;
        Anim_LoadKeyframe(&book->preview, &book->previewResource, (u16)book->animationIndex,
                          book->animationMode);
        Anim_SetKeyframes(&book->preview, &book->previewResource, (u16)book->animationIndex,
                          book->animationMode, 4);
    }
    book->frame = 0;
    return 0;
}
