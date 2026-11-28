#include "minigames/result_game/result_tutorial.h"

void ResultGame_HidePressMarker(struct ResultMenu *menu)
{
    AnimSprite_Hide(&menu->widgets[1]);
}
