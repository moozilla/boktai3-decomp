#include "global.h"
#define MovingSprite_UpdateMotion sub_08237450
#define MovingSprite_SetPhase MovingGlyph_SetState
struct MovingSpritePosition {
    u16 x, unknown2, y, unknown6;
};
struct MovingSprite {
    u8 unknown0[0x34];
    struct MovingSpritePosition displayedPosition;
    u8 unknown3C[0x24];
    struct MovingSpritePosition position;
    u8 unknown68[6];
    u16 velocityX, velocityY, unknown72, duration, frame;
    void (*phase)(void);
};
void MovingGlyph_SetState(struct MovingSprite *, void (*)(void));
void sub_082373D4(void);
void sub_08237450(struct MovingSprite *p)
{
    p->position.x += p->velocityX;
    p->position.y += p->velocityY;
    p->displayedPosition = p->position;
    p->frame++;
    if (p->frame >= p->duration)
        MovingGlyph_SetState(p, sub_082373D4);
}
