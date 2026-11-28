#include "global.h"
#include "ui/books/collection.h"

void Sprite_Remove(void *);
void Anim_Remove(void *);

void CollectionMenu_DestroyGraphics(struct CollectionMenu *menu)
{
    s32 i;

    for (i = 0; i < 27; i++)
        Sprite_Remove(&menu->entries[i]);
    for (i = 0; i < 4; i++)
        Anim_Remove(&menu->countDigits[i]);
    Anim_Remove(&menu->pageIndicator);
    Anim_Remove(&menu->completionBadge);
    Anim_Remove(&menu->cursor);
    Anim_Remove(&menu->background);
}
