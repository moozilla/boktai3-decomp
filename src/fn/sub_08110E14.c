#include "tilemap_panel.h"
s32 SceneTilemap_Update(struct TilemapPanel *panel)
{
    panel->scroll.previousX = panel->scroll.x;
    if (panel->scrollSource && panel->atEnd != 1)
        panel->scroll.x = *panel->scrollSource;
    TilemapPanel_StreamColumns(panel);
    return 0;
}
