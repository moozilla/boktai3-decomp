#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 58.5%
// Open: offset/constant register choice around the first store.
struct Glove {
    u8 pad0[0x864];
    u32 timer;
    u8 active;
    u8 pad869[3];
    void (*update)(void);
    u8 pad870[0x58];
    struct {
        u8 pad0[0x16];
        u16 flags;
    } draw;
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
        u32 one = 1;
        *active = one;
        glove->update = update;
        glove->timer = zero;
        glove->draw.flags |= one;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Glove_StartFromScript);
#endif
