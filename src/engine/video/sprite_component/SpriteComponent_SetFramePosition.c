#include "global.h"
#include "engine/video/sprite_component.h"

void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);

s32 SpriteComponent_SetFramePosition(struct SpriteComponent *component, s32 frame,
                                     const u32 *position)
{
    u32 x, y;
    if (component->frame > (frame & 0xFF))
        Sound_PlaySong(0x204);
    component->frame = frame & 0xFF;
    x = position[0];
    y = position[1];
    component->position[0] = x;
    component->position[1] = y;
    if (component->update)
        Callback_InvokeActor(component, component->update);
    return component->collisionStatus = 0;
}
