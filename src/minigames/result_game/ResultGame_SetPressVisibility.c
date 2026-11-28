#include "minigames/result_game/result_tutorial.h"

void ResultGame_SetPressVisibility(struct ResultMenu *menu)
{
    struct AnimSprite *sprite;
    sprite = &menu->widgets[5];
    sprite->flags |= 1;
    sprite = &menu->widgets[12];
    sprite->flags &= ~1;
}
