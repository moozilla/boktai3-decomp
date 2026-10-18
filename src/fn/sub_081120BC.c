#include "global.h"
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
extern const u8 gUnk_086106B0[];
struct SceneGauge {
    u8 unknown0[0x204];
    u8 sprite[0x8C4 - 0x204];
    u8 animation[0x8EE - 0x8C4];
    u16 frame;
};
s32 SceneGauge_SelectThresholdFrame(struct SceneGauge *s, s32 value)
{
    s32 level = -1;
    s32 i;
    for (i = 0; i < 4; i++) {
        if (value >= gUnk_086106B0[i]) {
            level = i;
            break;
        }
    }
    if (level < 0)
        level = 4;
    s->frame = level + 0x22;
    Anim_SelectWidgetGlyph(s->sprite, s->animation, s->frame);
    return level;
}
