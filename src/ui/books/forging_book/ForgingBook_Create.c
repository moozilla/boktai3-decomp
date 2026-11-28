#include "global.h"
#include "ui/books/forging_book.h"
s32 ForgingBook_InitializeFromScript(struct ForgingBook *, u32);
s32 sub_0821290C(struct ForgingBook *);
void ForgingBook_OnTaskDestroy(struct ForgingBook *);

void *ForgingBook_Create(u32 arg)
{
    struct ForgingBook *book = Task_Create(8, sizeof(struct ForgingBook));

    if (book) {
        Task_SetCallbacks(book, sub_0821290C, ForgingBook_OnTaskDestroy);
        if (ForgingBook_InitializeFromScript(book, arg) < 0) {
            Task_Kill(book);
            return 0;
        }
    }
    gForgingBook = (u32)book;
    return book;
}
