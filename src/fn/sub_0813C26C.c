#include "global.h"
/* Tentative role: initialize the orbiting effect and its update callback. */
typedef struct Point {
    u32 a, b;
} Point;
typedef struct Sprite {
    u32 flags;
    u8 pad4[3];
    u8 mode, xscale, yscale;
    u8 pada[6];
    u16 frame;
    u8 pad12[0xa];
    Point pos;
    u8 pad24[8];
} Sprite;
typedef struct State {
    u8 pad0[0x30];
    Point pos;
    u8 pad38[0x3e0];
    u8 mode;
    u8 pad419[0x3e3];
    Sprite sprite;
    u8 animation[0x38];
    u8 active;
    u8 pad861[3];
    void (*callback)(void *);
} State;
void AnimData_Load(void *, s32);
void Sprite_Init(Sprite *, void *, s32);
void AnimData_SelectTiles(void *, s32);
void sub_0813BFE0(void *);
void sub_0813C26C(State *s)
{
    Sprite *sprite = &s->sprite;
    void *animation = s->animation;
    AnimData_Load(animation, 0x2110);
    Sprite_Init(sprite, animation, 67);
    sprite->frame = 4;
    if (s->mode == 6)
        AnimData_SelectTiles(animation, 81);
    else
        AnimData_SelectTiles(animation, 75);
    sprite->mode = 1;
    sprite->yscale = 127;
    sprite->xscale = 127;
    sprite->pos = s->pos;
    s->callback = sub_0813BFE0;
    s->active = 0;
}
