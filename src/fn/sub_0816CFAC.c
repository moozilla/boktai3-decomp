#include "global.h"
#include "player.h"
#include "scene_hud_display.h"
s32 Div(s32, s32);
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *, s32, s32, s32, s32, s32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0813D834(struct Player *, s32, s32);
void SceneHud_ApplyStatBPercent(struct SceneHudState *s, s32 percent, s32 duration)
{
    struct Player *p = s->actionOwner;
    s32 value;
    if (p) {
        s->appliedStatB = Div(p->maximumStatB * percent, 100);
        value = p->currentStatB + Div(p->maximumStatB * percent, 100);
        if (value > p->maximumStatB) {
            s->appliedStatB = p->maximumStatB - p->currentStatB;
            p->currentStatB = p->maximumStatB;
        } else if (value < 0) {
            p->currentStatB = 0;
        } else {
            p->currentStatB = value;
        }
        if (s->appliedStatB < 0)
            s->appliedStatB = 0;
        Hud_DrawDefaultEnergyMeter(s, 1, 2, 14, s->actionOwner->currentStatB, s->actionOwner->maximumStatB);
        if (duration > 0) {
            if (Inventory_GetQuantityByStorage(s->actionCategory, s->actionSelection) == 14)
                sub_0813D834(p, 1, duration * 60);
            else
                sub_0813D834(p, 0, duration * 60);
        }
    }
}
