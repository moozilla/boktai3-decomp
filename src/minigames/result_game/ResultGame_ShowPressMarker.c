#include "minigames/result_game/result_tutorial.h"

void ResultGame_ShowPressMarker(struct ResultMenu *menu)
{
    struct AnimSprite *sprite = &menu->widgets[1];
    sprite->position.x = menu->pressScore;
    AnimSprite_Show(sprite);
}
