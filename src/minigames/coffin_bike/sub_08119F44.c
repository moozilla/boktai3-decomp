#include "global.h"

#include "engine/engine.h"
#include "link/wireless/scene_pool.h"
#include "minigames/coffin_bike/glove.h"
void Glove_UpdateStopping(struct Glove *);
static inline void clearDrawFlags(struct GloveDraw *d, s32 mask)
{
    d->flags &= mask;
}
/* Registry routine (Glove): glove -i (default 0) switches to update
   Glove_UpdateStopping and clears draw flag 1 (counterpart of Glove_StartFromScript). */
void Glove_StopFromScript(void)
{
    struct Glove *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gParticipantActors[i];
    if (glove) {
        void (*update)(void *) = (void (*)(void *))Glove_UpdateStopping;
        u8 *active = &glove->active;
        u32 zero = 0;
        *active = 1;
        glove->update = update;
        glove->timer = zero;
        clearDrawFlags(&glove->draw, ~1);
    }
}
