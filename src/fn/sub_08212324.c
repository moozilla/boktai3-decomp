#include "global.h"
#include "render.h"

struct ForgingBook {
    u8 unknown[0xAA8];
    s16 previewX;
    u8 unknownAAA[2];
    s16 previewY;
    u8 unknownAAE[0xB2];
    s16 category;
};
void ForgingBook_RefreshEntrySprites(struct ForgingBook *);
void SceneKeyframe_UpdateSelectionVisibility(struct ForgingBook *);
void ForgingBook_RefreshCategoryPreview(struct ForgingBook *);
void ForgingBook_UpdateSelectionCursor(struct ForgingBook *);
void ForgingBook_UpdateCategoryCountDigits(struct ForgingBook *);
void ForgingBook_RefreshSelectedEntryText(struct ForgingBook *);

s32 ForgingBook_RefreshCategory(struct ForgingBook *book)
{
    /* Retained scratch space accounts for the original four-byte frame. */
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
            book->previewX = ((u16)gCamera.position.x) - 466;
            book->previewY = ((u16)gCamera.position.z) + 308;
        } else {
            book->previewX = ((u16)gCamera.position.x) - 456;
            book->previewY = ((u16)gCamera.position.z) + 336;
        }
        ForgingBook_RefreshCategoryPreview(book);
    }
    return 0;
}
