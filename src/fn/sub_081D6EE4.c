#include "global.h"
#include "monster_book.h"

void BackgroundPanel_ReleaseHandles(struct MonsterBook *);
s32 Bestiary_GetCategoryFirstEntry(u32);
s32 Bestiary_GetCategoryEntryCount(u32);
s32 TextWindow_SetScript(s32, const u8 *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(s32);
s32 GameState_IsEventFlagSet(s32);

void sub_081D6EE4(struct MonsterBook *p)
{
    s32 i;
    s32 first, count;

    BackgroundPanel_ReleaseHandles(p);
    first = Bestiary_GetCategoryFirstEntry(p->category);
    count = Bestiary_GetCategoryEntryCount(p->category);
    for (i = 0; i < count; i++) {
        s32 e;
        TextWindow_SetScript(p->windows[i], p->script);
        TextWindow_SetBounds(p->windows[i], 6, i * 2 + 5, 0x14, 2);
        e = first + i;
        if (GameState_IsEventFlagSet(e))
            TextWindow_Refresh(p->windows[i], e);
        else
            TextWindow_Refresh(p->windows[i], 0x53);
        TextWindow_StartPlayback(p->windows[i]);
    }
}
