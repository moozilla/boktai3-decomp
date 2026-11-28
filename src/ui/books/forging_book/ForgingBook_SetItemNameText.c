#include "global.h"
#include "ui/books/forging_book.h"
s32 ScrollingDialogue_SetScript(const u8 *);

void ForgingBook_SetItemNameText(const u8 *entry, const void *script)
{
    s32 id = Script_ParseStringRef(script) + 0x30;

    ScrollingDialogue_SetScript(Text_LookupString(id + *entry));
}
