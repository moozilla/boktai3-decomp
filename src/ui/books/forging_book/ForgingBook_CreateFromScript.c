#include "global.h"
#include "ui/books/collection_book.h"
s32 ForgingBook_InitializePageFromScript(struct CollectionBook *, u32);
s32 CollectionBook_UpdateMenu(struct CollectionBook *);
u32 sub_08213B50(struct CollectionBook *);

void *ForgingBook_CreateFromScript(u32 arg)
{
    struct CollectionBook *book = Task_Create(8, sizeof(struct CollectionBook));

    if (book) {
        Task_SetCallbacks(book, CollectionBook_UpdateMenu, sub_08213B50);
        if (ForgingBook_InitializePageFromScript(book, arg) < 0) {
            Task_Kill(book);
            return 0;
        }
    }
    return book;
}
