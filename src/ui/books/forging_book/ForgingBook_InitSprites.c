#include "global.h"
#include "ui/books/forging_book.h"
#include "engine/video/render.h"
void AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, u32, const void *);
s32 Mod(s32, s32);
void *ActorDebris_Create(void *, void *, void *);
void *PlayerAttackEffect_Create(void *, void *, void *);

u32 ForgingBook_InitSprites(struct ForgingBook *book)
{
    struct AnimSpriteResource *resource, *previewResource;
    struct ContactPosition position;
    struct DisplaySequenceSprite *sprite;
    s32 i;

    resource = ScriptProc_Start(0xcb05, 0xe2ab);
    book->resource = *resource;
    ResourceHeader_Relocate(&book->resource, resource);
    AnimSprite_InitLinked(&book->background, &book->resource, 0x49, 0x10, 0, 0, 0, 0);
    AnimSprite_InitLinked(&book->completionBadge, &book->resource, 0x3e, 0x10, 0, 0, 0, 0);
    if (!ForgingBook_IsCategoryComplete(book, book->category))
        book->completionBadge.flags |= 1;
    position.x = 0xb8;
    position.y = 0x68;
    position.z = 0;
    for (i = 0; i < 4; i++) {
        AnimSprite_InitLinked(&book->countDigits[i], &book->resource, 0, 0x10, 0, 0, 0, &position);
        position.x += 8;
        if (i == 1)
            position.x += 8;
    }
    AnimSprite_InitLinked(&book->categoryHeading, &book->resource, 0x4c, 0x10, 0, 0, 0, 0);
    resource = ScriptProc_Start(0xcb05, 0x530d);
    book->cursorResource = *resource;
    ResourceHeader_Relocate(&book->cursorResource, resource);
    position.x = 0x58;
    position.y = 0x18;
    position.z = 0;
    AnimSprite_InitLinked(&book->cursor, &book->cursorResource, 0x4d, 0x10, 0, 0, 0, &position);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    book->cellResource = *resource;
    ResourceHeader_Relocate(&book->cellResource, resource);
    position.x = 0x58;
    position.y = 0x18;
    for (i = 0; i < 18; i++) {
        AnimSprite_InitLinked(&book->cells[i], &book->cellResource, (u16)(0xf0 + i), 0x10, 0, 0, 0,
                              0);
        book->cells[i].position.x = position.x;
        book->cells[i].position.y = position.y;
        book->cells[i].position.z = 0;
        if (Mod(i, 6) == 5) {
            position.x = 0x58;
            position.y += 0x18;
        } else {
            position.x += 0x18;
        }
    }
    CpuSet(book->cursor.palette, book->cursorColors, 0x10);
    book->cursor.palette = book->cursorColors;
    book->cursor.paletteBank = 0xe010;
    previewResource = ScriptProc_Start(0xcb05, 0xde23);
    ResourceHeader_Relocate(&book->previewResource, previewResource);
    AnimSprite_InitLinked(&book->preview, &book->previewResource, 0, 0, 0, 1, 0, 0);
    book->preview.flags &= ~1;
    book->preview.attribute1A = 2;
    CpuSet(book->preview.palette, book->previewColors, 0x10);
    book->preview.palette = book->previewColors;
    book->preview.position.x = gCamera.position.x - 456;
    book->preview.position.y = gCamera.position.y;
    book->preview.position.z = gCamera.position.z + 336;
    book->animationBase = 3;
    book->animationFlag = 1;
    book->debrisTask =
        ActorDebris_Create(&book->preview, &book->animationBase, &book->animationFlag);
    book->attackEffectTask =
        PlayerAttackEffect_Create(&book->preview, &book->unknownBE6, &book->animationFlag);
    AnimData_Load(&book->spriteGraphics, 0x1c1b);
    sprite = &book->sprite;
    Sprite_Init(sprite, &book->spriteGraphics, 0x212);
    sprite->frame = 5;
    book->sprite.priority = 1;
    book->sprite.position.x = 0x2e;
    book->sprite.position.y = 0x48;
    book->sprite.position.z = 0;
    book->sprite.palette = 0x40;
    book->sprite.paletteVariant = 0x40;
}
