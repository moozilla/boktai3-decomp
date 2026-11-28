#include "minigames/result_game/result_tutorial.h"

void ResultGame_HideTargetMarker(struct ResultMenu *menu)
{
    AnimSprite_Hide(&menu->widgets[3]);
}
