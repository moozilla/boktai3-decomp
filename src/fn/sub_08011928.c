#include "scene_indicator.h"
void SceneIndicator_Update(struct SceneIndicators *owner, struct SceneIndicator *p)
{
    u8 old;
    if (p->disabled) {
        if (p->visible) {
            p->visible = 0;
            IndicatorSprite_SetFrame(&p->sprite, p->frame);
        }
        return;
    }
    old = p->visible;
    switch (p->mode) {
        case 0:
            if (p->timer >= (u32)(s16)p->times[p->index]) {
                p->index++;
                if (p->index >= p->count)
                    p->index = 0;
                p->visible = 1 - (p->index & 1);
                if (p->visible)
                    Sound_PlaySongChecked(0x5AA);
                p->timer = 0;
            }
            break;
        case 1:
            if (!old) {
                if (!p->delay) {
                    if (p->value == owner->tile)
                        p->delay = 1;
                } else {
                    p->delay++;
                    if (p->delay >= p->period) {
                        p->visible = 1;
                        p->delay = 60;
                        Sound_PlaySongChecked(0x5AA);
                    }
                }
            } else {
                if (!p->delay)
                    p->visible = 0;
                else
                    p->delay--;
            }
            break;
        case 2:
            break;
    }
    {
        int visible = p->visible;
        if (old != visible)
            IndicatorSprite_SetFrame(&p->sprite, p->frame + visible);
    }
    if (p->visible == 1)
        Collider_Refresh(p->effect);
    p->timer++;
}
