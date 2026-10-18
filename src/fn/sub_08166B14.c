#define MenuEntries_CountAvailable sub_08166B14
#define MenuEntries_IsAvailable PauseMenu_GetCategoryEntryValue
#include "global.h"
s32 MenuEntries_IsAvailable(u32, s32);
s32 sub_08166B14(void *unused, s32 category)
{
    s32 count = 0;
    s32 i;
    switch (category) {
        case 2:
            for (i = 0; i < 16; i++)
                if (MenuEntries_IsAvailable(i, category))
                    count++;
            break;
        case 1:
            for (i = 8; i < 20; i++)
                if (MenuEntries_IsAvailable(i, category))
                    count++;
            break;
        case 0:
            for (i = 0; i < 8; i++)
                if (MenuEntries_IsAvailable(i, category))
                    count++;
            break;
    }
    return count;
}
