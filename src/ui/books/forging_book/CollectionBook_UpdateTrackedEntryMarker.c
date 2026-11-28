#include "global.h"
#include "ui/books/collection_book.h"
s32 Lookup_FindValueIndex(struct CollectionBook *, s32);
s32 Div(s32, s32);
s32 Mod(s32, s32);

u32 CollectionBook_UpdateTrackedEntryMarker(struct CollectionBook *book)
{
    s32 index = Lookup_FindValueIndex(book, book->trackedEntry);

    if (index >= book->page * 10 && index < (book->page + 1) * 10) {
        s32 cell = Mod(index, 10);
        s32 column = Mod(cell, 5);
        s32 row = Div(cell, 5);

        book->trackedMarker.position.x = column * 40 + 38;
        book->trackedMarker.position.y = row * 40 + 38;
        book->trackedMarker.flags &= ~1;
        Anim_LoadKeyframe(&book->trackedMarker, &book->markerResource, 0x50, 0);
    } else {
        book->trackedMarker.flags |= 1;
    }
    return 0;
}
