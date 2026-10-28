#include "global.h"
#include "engine.h"
#include "scene_pool.h"
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
void Glove_UpdateRecoil(void);
/* Registry routine E3EE (Glove): starts glove -i (default 0): active, update
   Glove_UpdateRecoil, timer 0, draw flag 1. */
void Glove_StartFromScript(void)
{
    struct Glove *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gParticipantActors[i];
    if (glove) {
        void (*update)(void) = Glove_UpdateRecoil;
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
