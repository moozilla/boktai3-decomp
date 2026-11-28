#include "minigames/result_game/result_tutorial.h"
void Anim_Remove(void *);
void Effect_Remove(void *);

int ResultMenu_DestroyGraphics(struct ResultMenu *menu)
{
    int i;
    for (i = 0; i < 15; i++)
        Anim_Remove(&menu->widgets[i]);
    for (i = 0; i < 4; i++)
        Effect_Remove(&menu->markers[i]);
}
