#include "global.h"
#include "glove.h"
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Glove_SetAnimationKeyframes(struct Glove *s, u32 x)
{
    Anim_SetKeyframes(&s->model.parts[2], s->model.primaryAnimation, (u16)x, 1, 0);
}
