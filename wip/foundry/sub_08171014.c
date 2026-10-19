#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#define SceneHud_DrawComparisonBars  sub_08171014
#define SceneHud_RenderComparisonBar sub_081664A8
u16 *sub_08163F70(s32, s32, s32);
void sub_081664A8(s32, s32, void *, struct SceneInputWidget *, struct SceneInputWidget *,
                  struct SceneInputWidget *);
void sub_08171014(struct SceneHudState *p, s32 x, s32 y, s16 *limits)
{
    s32 i, j;
    for (i = 0; i < 3; i++) {
        u16 *tiles;
        p->comparisonValues[i] =
            *(u8 *)&p->actionOwner->derivedStats[i] + *(u8 *)&p->actionOwner->menuSnapshotValues[i];
        if (p->comparisonValues[i] > 120)
            p->comparisonValues[i] = 120;
        tiles = sub_08163F70(0, x, y + i);
        for (j = 2; j >= 0; j--)
            *tiles++ = 0xF001;
        sub_081664A8(p->comparisonValues[i] + p->comparisonOffsets[i], limits[i], p->widgetResource,
                     &p->motionWidgets[i * 3 + 2], &p->motionWidgets[i * 3 + 3],
                     &p->motionWidgets[i * 3 + 4]);
        {
            struct ContactPosition *sprite = &p->motionWidgets[i * 3 + 2].position;
            sprite->x = x * 8;
            sprite->y = (y + i) * 8;
            sprite->z = 0;
            sprite = &p->motionWidgets[i * 3 + 3].position;
            sprite->x = x * 8 + 8;
            sprite->y = (y + i) * 8;
            sprite->z = 0;
            sprite = &p->motionWidgets[i * 3 + 4].position;
            sprite->x = x * 8 + 16;
            sprite->y = (y + i) * 8;
            sprite->z = 0;
            p->motionWidgets[i * 3 + 2].flags &= ~1;
            p->motionWidgets[i * 3 + 3].flags &= ~1;
            p->motionWidgets[i * 3 + 4].flags &= ~1;
        }
    }
}
