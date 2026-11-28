#include "global.h"
#include "ui/books/bestiary_display.h"
/* BestiaryDisplay_OnTaskDestroy: Release active animation channels and shared sprite */

void DisplayAnimation_Destroy(struct BestiaryAnimation *);
void Sprite_Remove(struct DisplaySequenceSprite *);

s32 BestiaryDisplay_OnTaskDestroy(struct BestiaryDisplay *p)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (p->animations[i].active)
            DisplayAnimation_Destroy(&p->animations[i]);
    }
    Sprite_Remove(&p->sprite);
    return 0;
}
