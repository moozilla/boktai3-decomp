#include "global.h"
#include "engine/video/sprite_component.h"

void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);

s32 SpriteComponent_SetFramePosition(struct SpriteComponent *component, s32 frame,
                                     const struct ContactPosition *position)
{
    if (component->frame > (frame & 0xFF))
        Sound_PlaySong(0x204);
    component->frame = frame & 0xFF;
    component->position = *position;
    if (component->update)
        Callback_InvokeActor(component, component->update);
    return component->collisionStatus = 0;
}
