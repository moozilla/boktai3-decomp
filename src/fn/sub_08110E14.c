#include "tilemap_panel.h"
s32 SceneTilemap_Update(struct TilemapPanel *panel)
{
    panel->scroll.previousX = panel->scroll.x;
    if (panel->scrollSource && panel->atEnd != 1)
        panel->scroll.x = *panel->scrollSource;
    sub_08110C9C(panel);
    return 0;
}
