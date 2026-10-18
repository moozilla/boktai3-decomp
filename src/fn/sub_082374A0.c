#include "global.h"
#define MovingSprite_SetPhase MovingGlyph_SetState
struct MovingSprite {
    u8 task[0x18];
    u32 flags;
    u8 unknown1C[0x56];
    u16 delay, duration, frame;
    void (*phase)(void);
};
void MovingGlyph_SetState(struct MovingSprite *, void (*)(void));
void MovingSprite_UpdateMotion(void);
void MovingSprite_UpdateDelay(struct MovingSprite *p)
{
    p->frame++;
    if (p->frame >= p->delay) {
        p->flags &= ~1;
        MovingGlyph_SetState(p, MovingSprite_UpdateMotion);
    }
}
