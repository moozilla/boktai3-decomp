#include "global.h"
#include "engine.h"
struct GloveDraw {
    u8 pad0[0x16];
    u16 flags;
};
struct Glove {
    u8 pad0[0x864];
    u32 timer;
    u8 active;
    u8 pad869[3];
    void (*update)(void);
    u8 pad870[0x58];
    struct GloveDraw draw;
};
extern struct Glove *gUnk_020004F0[];
void sub_081188F4(void);
/* Registry routine E3EE (Glove): starts glove -i (default 0): active, update
   sub_081188F4, timer 0, draw flag 1. */
void Glove_StartFromScript(void)
{
    struct Glove *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gUnk_020004F0[i];
    if (glove) {
        void (*update)(void) = sub_081188F4;
        u8 *active = &glove->active;
        u32 zero = 0;
        u16 one = 1;
        *active = one;
        glove->update = update;
        glove->timer = zero;
        {
            struct GloveDraw *draw = &glove->draw;
            draw->flags = one | draw->flags;
        }
    }
}
