#include "global.h"
#include "ui/town/storage_screen.h"
/* Storage menu; initializes six identity-mapped entries and option e. */
s32 Script_GetValue(void);
void Storage_InitEntries(struct StorageScreen *screen)
{
    s32 i;
    s32 found;
    screen->category = 0;
    screen->categoryCount = 6;
    for (i = 0; i < screen->categoryCount; i++)
        screen->categoryKinds[i] = i;
    found = Script_SeekToKeyword(0x65);
    if (found)
        screen->completionScript = Script_GetValue();
    else
        screen->completionScript = found;
}
