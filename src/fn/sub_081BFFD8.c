#include "global.h"

s32 DirectionMarker_GetAnimation(s32 base, s32 group);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
s32 Anim_Step(void *anim, void *data);

typedef struct {
    u8 pad_00[0x18];
    u8 anim[0x78 - 0x18];
    u8 data[0x98 - 0x78];
    s16 dx;
    s16 dy;
    u8 pad_9C[0xA0 - 0x9C];
    s16 curAnimIndex;
    s16 pad_A2;
    s32 category;
} DirectionMarker;

s32 DirectionMarker_Update(DirectionMarker *this)
{
    s32 quadrant;
    s32 newIndex;
    s16 *p = &this->dx;

    if (p[0] > 0x77) {
        if (p[1] > 0x4f) {
            quadrant = 1;
        } else {
            quadrant = 0;
        }
    } else {
        if (p[1] > 0x4f) {
            quadrant = 2;
        } else {
            quadrant = 3;
        }
    }

    newIndex = DirectionMarker_GetAnimation(quadrant, this->category);
    if (newIndex != this->curAnimIndex) {
        this->curAnimIndex = newIndex;
        Anim_SetKeyframes(this->anim, this->data, (u16)this->curAnimIndex, 1, 0);
    }
    Anim_Step(this->anim, this->data);
    return quadrant;
}
