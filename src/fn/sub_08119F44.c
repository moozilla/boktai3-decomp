#include "global.h"

#include "engine.h"
struct GloveDraw {
    u8 pad0[0x16];
    s16 flags;
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
void sub_08117948(void);
static inline void clearDrawFlags(struct GloveDraw *d, s32 mask)
{
    d->flags &= mask;
}
/* Registry routine (Glove): glove -i (default 0) switches to update
   sub_08117948 and clears draw flag 1 (counterpart of Glove_StartFromScript). */
void Glove_StopFromScript(void)
{
    struct Glove *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gUnk_020004F0[i];
    if (glove) {
        void (*update)(void) = sub_08117948;
        u8 *active = &glove->active;
        u32 zero = 0;
        u16 one = 1;
        *active = one;
        glove->update = update;
        glove->timer = zero;
        clearDrawFlags(&glove->draw, ~1);
    }
}
