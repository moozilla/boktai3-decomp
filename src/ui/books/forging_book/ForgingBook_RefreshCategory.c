#include "global.h"
#include "ui/books/forging_book.h"
#include "engine/video/render.h"
void ForgingBook_RefreshEntrySprites(struct ForgingBook *);
void SceneKeyframe_UpdateSelectionVisibility(struct ForgingBook *);
void ForgingBook_RefreshCategoryPreview(struct ForgingBook *);
void ForgingBook_UpdateSelectionCursor(struct ForgingBook *);
void ForgingBook_UpdateCategoryCountDigits(struct ForgingBook *);
void ForgingBook_RefreshSelectedEntryText(struct ForgingBook *);

s32 ForgingBook_RefreshCategory(struct ForgingBook *book)
{
    /* Unused in the original too: it accounts for the four-byte stack frame. */
    s16 unused[2];

    ForgingBook_RefreshEntrySprites(book);
    ForgingBook_UpdateSelectionCursor(book);
    ForgingBook_UpdateCategoryCountDigits(book);
    SceneKeyframe_UpdateSelectionVisibility(book);
    ForgingBook_RefreshSelectedEntryText(book);
    if (book->category <= 5) {
        ForgingBook_RefreshCategoryPreview(book);
    } else {
        if (book->category == 8) {
            book->preview.position.x = gCamera.position.x - 466;
            book->preview.position.z = gCamera.position.z + 308;
        } else {
            book->preview.position.x = gCamera.position.x - 456;
            book->preview.position.z = gCamera.position.z + 336;
        }
        ForgingBook_RefreshCategoryPreview(book);
    }
    return 0;
}
