#include "global.h"

#include "engine/engine.h"
#include "link/wireless/scene_pool.h"
#include "minigames/coffin_bike/coffin_bike.h"
void CoffinBike_UpdateStopping(struct CoffinBike *);
static inline void clearDrawFlags(struct CoffinBikeDraw *d, s32 mask)
{
    d->flags &= mask;
}
/* Registry routine (CoffinBike): glove -i (default 0) switches to update
   CoffinBike_UpdateStopping and clears draw flag 1 (counterpart of CoffinBike_StartFromScript). */
void CoffinBike_StopFromScript(void)
{
    struct CoffinBike *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gParticipantActors[i];
    if (glove) {
        void (*update)(void *) = (void (*)(void *))CoffinBike_UpdateStopping;
        u8 *active = &glove->active;
        u32 zero = 0;
        *active = 1;
        glove->update = update;
        glove->timer = zero;
        clearDrawFlags(&glove->draw, ~1);
    }
}
