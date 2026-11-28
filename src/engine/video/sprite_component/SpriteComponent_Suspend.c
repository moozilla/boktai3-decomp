#include "global.h"
#include "engine/video/sprite_component.h"
void Sound_StopSong(u32);
u32 SpriteComponent_Suspend(struct SpriteComponent *component)
{
    if (!(component->sprite.flags & 1))
        Sound_StopSong(0x202);
    component->sprite.flags |= 1;
    /* Callers ignore the unspecified ABI result. */
}
