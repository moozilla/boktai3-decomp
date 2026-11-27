#include "global.h"
#include "minigames/coffin_bike/coffin_bike.h"
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void CoffinBike_SetAnimationKeyframes(struct CoffinBike *s, u32 x)
{
    Anim_SetKeyframes(&s->model.parts[2], s->model.primaryAnimation, (u16)x, 1, 0);
}
