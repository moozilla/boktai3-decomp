#include "global.h"

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
    u8 animation[0x2c];
    void *script;
    u8 pad854b[8];
    u8 active;
    u8 pad861[3];
    void (*callback)(void *);
} State;
void AnimData_Load(void *, s32);
void Sprite_Init(Sprite *, void *, s32);
void AnimData_SelectPaletteBank(void *, s32);
void *ScriptProc_Start(u16, u16);
void Player_OnDirectionalEffectAnimationEvent(void *);
void DirectionalEffect_Update(void *);

void DirectionalEffect_Init(State *s)
{
    Sprite *sprite = &s->sprite;
    void *animation = s->animation;
    if (s->mode == 6) {
        AnimData_Load(animation, 0x2110);
        Sprite_Init(sprite, animation, 67);
        sprite->frame = 4;
        AnimData_SelectPaletteBank(animation, 81);
        sprite->mode = 1;
        sprite->yscale = 127;
        sprite->xscale = 127;
        sprite->pos = s->pos;
        s->callback = Player_OnDirectionalEffectAnimationEvent;
    } else {
        AnimData_Load(animation, 0x8F5D);
        Sprite_Init(sprite, animation, 1);
        AnimData_SelectPaletteBank(animation, 42);
        s->script = ScriptProc_Start(0x922E, 0x837);
        s->callback = DirectionalEffect_Update;
    }
    s->active = 0;
}
