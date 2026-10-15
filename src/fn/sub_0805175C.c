#include "breakable_prop.h"
void BreakableProp_InitializeSprite(struct BreakableProp *prop, void *packet)
{
    struct ContactSwarmSprite *sprite = &prop->sprite;
    u8 *graphics = prop->graphics;
    s32 frame;
    if (Script_SeekToKeyword('t'))
        AnimData_Load(graphics, Script_GetValue());
    else
        AnimData_Load(graphics, 0x9D41);
    Sprite_Init(sprite, graphics, 0);
    frame = Script_SeekToKeyword('i');
    if (frame)
        frame = Script_GetValue();
    sprite->frame = frame;
    sprite->position = *(struct ContactPosition *)packet;
    if (Script_SeekToKeyword('r') && Script_GetValue())
        sprite->flags |= 4;
}
