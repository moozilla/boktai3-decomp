#include "global.h"
#include "render.h"
#define gRandomTableIndex gUnk_03005308
#define gRandomTable      gUnk_0203B400

struct ScreenPaletteTransition {
    u8 unknown0[4];
    u8 *palettes;
    s16 *position;
    u8 unknownC[8];
    u8 variant;
    u8 state;
    u16 phase;
};
extern u32 gUnk_02000560;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 ScalarApproach_StepSixteenth(s32, s32);
s32 sub_08249350(s32, s32);
void ScreenPalette_UpdateTransition(struct ScreenPaletteTransition *);
void ScreenPalette_ProjectPosition(int, s16 *, int, int);

static inline int nextRandom(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}

void ScreenPalette_UpdateState(struct ScreenPaletteTransition *s)
{
    s32 jitter;
    u32 mask = 4;
    if (gUnk_02000560 & mask)
        goto hide;
    switch (s->state) {
        case 0:
        hide: {
            u16 *control = &gDispCntShadow;
            u32 clear = ~0x200;
            *control = clear & *control;
        }
            return;
        case 1:
            if (s->phase == 0)
                s->phase++;
            break;
        case 2:
            s->phase = ScalarApproach_StepSixteenth(s->phase, 96);
            if (s->phase > 95)
                s->state = 3;
            break;
        case 4:
            if (--s->phase == 0) {
                s->state = 0;
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
    if (s->state > 1) {
        jitter = sub_08249350(nextRandom() >> 3, 3) - 1;
    } else {
        jitter = 0;
    }
    ScreenPalette_ProjectPosition(1, s->position, jitter + 136, jitter - 70);
}
