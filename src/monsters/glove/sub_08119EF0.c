#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/scene_pool.h"
#include "monsters/glove/glove.h"
void Glove_UpdateRecoil(struct Glove *);
/* Registry routine E3EE (Glove): starts glove -i (default 0): active, update
   Glove_UpdateRecoil, timer 0, draw flag 1. */
void Glove_StartFromScript(void)
{
    struct Glove *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gParticipantActors[i];
    if (glove) {
        void (*update)(void *) = (void (*)(void *))Glove_UpdateRecoil;
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
