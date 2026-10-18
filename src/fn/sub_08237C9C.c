#include "global.h"
#define TimedSprite_UpdateMotion sub_08237C9C
#define TimedSprite_SetPhase ActorGlyphTrail_SetState
struct TimedSprite {
    u8 unknown0[0x38];
    u16 x, unknown3A, y, unknown3E;
    u8 unknown40[0x24];
    u16 velocityX, unknown66, velocityY;
    u8 unknown6A[6];
    u16 frame, duration;
};
void ActorGlyphTrail_SetState(struct TimedSprite *, void (*)(void));
void sub_08237CDC(void);
void sub_08237C9C(struct TimedSprite *p)
{
    p->x += p->velocityX;
    p->y += p->velocityY;
    p->frame++;
    if (p->frame >= p->duration)
        ActorGlyphTrail_SetState(p, sub_08237CDC);
}
