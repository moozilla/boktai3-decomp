#include "minigames/result_game_tutorial/result_tutorial.h"
#include "engine/shared/sprite_drawable_api.h"

void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);

int ResultMenu_CreateGraphics(struct ResultMenu *menu)
{
    struct ContactPosition position;
    const struct SpriteEffectTileResource *resource;
    int i;

    ZeroFill8(&position);
    for (i = 0; i < 15; i++)
        Widget_InitLinked(&menu->widgets[i], &menu->primaryResource, 0xb9, 0x11, 0, 0, 60,
                          &position);

    resource = AnimData_FindEntry(0x1c1a);
    for (i = 0; i < 4; i++)
        ParticleSprite_Initialize(&menu->markers[i], resource, 0x11);
    menu->hideLevel = 0;
    menu->hideTimer = 0;
}
