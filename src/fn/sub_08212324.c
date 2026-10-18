#include "global.h"
#define ForgingBook_RefreshCategory sub_08212324
#define ForgingBook_RefreshEntrySprites sub_08211AC4
#define ForgingBook_UpdateCategoryLabel sub_08211D3C
#define ForgingBook_RefreshCategoryPreview sub_08211DE4
#define gCameraPosition gUnk_030042A0

struct ForgingBook {
    u8 unknown[0xAA8];
    s16 previewX;
    u8 unknownAAA[2];
    s16 previewY;
    u8 unknownAAE[0xB2];
    s16 category;
};
struct ForgingBookCameraPosition { u16 x, unknown, y; };
extern struct ForgingBookCameraPosition gUnk_030042A0;
void sub_08211AC4(struct ForgingBook *);
void sub_08211D3C(struct ForgingBook *);
void sub_08211DE4(struct ForgingBook *);
void ForgingBook_UpdateSelectionCursor(struct ForgingBook *);
void ForgingBook_UpdateCategoryCountDigits(struct ForgingBook *);
void ForgingBook_RefreshSelectedEntryText(struct ForgingBook *);

s32 sub_08212324(struct ForgingBook *book)
{
    /* Retained scratch space accounts for the original four-byte frame. */
    s16 unused[2];
    sub_08211AC4(book);
    ForgingBook_UpdateSelectionCursor(book);
    ForgingBook_UpdateCategoryCountDigits(book);
    sub_08211D3C(book);
    ForgingBook_RefreshSelectedEntryText(book);
    if (book->category <= 5) {
        sub_08211DE4(book);
    } else {
        if (book->category == 8) {
            book->previewX = gUnk_030042A0.x - 466;
            book->previewY = gUnk_030042A0.y + 308;
        } else {
            book->previewX = gUnk_030042A0.x - 456;
            book->previewY = gUnk_030042A0.y + 336;
        }
        sub_08211DE4(book);
    }
    return 0;
}
