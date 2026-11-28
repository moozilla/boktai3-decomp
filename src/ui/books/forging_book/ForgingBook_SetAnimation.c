#include "global.h"
#include "ui/books/forging_book.h"
void Anim_SetFrameDurationScale(void *, u32);

s32 ForgingBook_SetAnimation(struct ForgingBook *book, s32 index, s32 scale, s32 mode)
{
    s32 same = 0;
    struct AnimSprite *preview;
    struct AnimSpriteResource *resource;
    s32 sequence;

    if (book->animationIndex == index)
        same = 1;
    book->animationIndex = index;
    preview = &book->preview;
    resource = &book->previewResource;
    sequence = index + book->animationBase;
    Anim_SetKeyframes(preview, resource, (u16)sequence, (u16)mode, same);
    if (book->animationFlag)
        book->preview.flags |= 4;
    else
        book->preview.flags &= ~4;
    Anim_SetFrameDurationScale(&book->preview, (u16)scale);
    return Anim_Step(&book->preview, &book->previewResource);
}
