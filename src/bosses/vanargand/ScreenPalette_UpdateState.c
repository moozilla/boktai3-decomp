#include "global.h"
#include "engine/video/render.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "effects/bat_flock.h"
#include "bosses/vanargand/screen_palette_transition_layout.h"
#define gRandomTableIndex gRandomIndex

extern u32 gUnk_02000560;
s32 ScalarApproach_StepSixteenth(s32, s32);
s32 sub_08249350(s32, s32);
void ScreenPalette_UpdateTransition(struct ScreenPaletteTransition *);
void ScreenPalette_ProjectPosition(int, s16 *, int, int);

void ScreenPalette_UpdateState(struct ScreenPaletteTransition *s)
{
    s32 jitter;
    u32 mask = 4;
    if (gUnk_02000560 & mask)
        goto hide;
    switch (s->enabled) {
        case 0:
        hide: {
            u16 *control = &gDispCntShadow;
            u32 clear = ~0x200;
            *control = clear & *control;
        }
            return;
        case 1:
            if (s->timer == 0)
                s->timer++;
            break;
        case 2:
            s->timer = ScalarApproach_StepSixteenth(s->timer, 96);
            if (s->timer > 95)
                s->enabled = 3;
            break;
        case 4:
            if (--s->timer == 0) {
                s->enabled = 0;
                return;
            }
            break;
        case 3:
            break;
    }
    {
        u32 visible = 0x200;
        gDispCntShadow = visible | gDispCntShadow;
    }
    ScreenPalette_UpdateTransition(s);
    if (s->enabled > 1) {
        jitter = sub_08249350(randomValue() >> 3, 3) - 1;
    } else {
        jitter = 0;
    }
    ScreenPalette_ProjectPosition(1, s->coordinates, jitter + 136, jitter - 70);
}
