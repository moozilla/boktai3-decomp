#include "global.h"
#include "engine/engine.h"
#include "link/wireless/scene_pool.h"
#include "minigames/coffin_bike/coffin_bike.h"
void CoffinBike_UpdateRecoil(struct CoffinBike *);
/* Registry routine E3EE (CoffinBike): starts glove -i (default 0): active, update
   CoffinBike_UpdateRecoil, timer 0, draw flag 1. */
void CoffinBike_StartFromScript(void)
{
    struct CoffinBike *glove;
    s32 i = Script_GetKeywordValue('i', 0);
    glove = gParticipantActors[i];
    if (glove) {
        void (*update)(void *) = (void (*)(void *))CoffinBike_UpdateRecoil;
        u8 *active = &glove->active;
        u32 zero = 0;
        u16 one = 1;
        *active = one;
        glove->update = update;
        glove->timer = zero;
        {
            struct CoffinBikeDraw *draw = &glove->draw;
            draw->flags = one | draw->flags;
        }
    }
}
