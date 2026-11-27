#include "global.h"
struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
    u32 flags;
};
void Sound_StopSong(u32);
u32 SpriteComponent_Suspend(struct SpriteComponent *component)
{
    u32 flags = component->flags;
    u32 active = flags & 1;
    if (!active)
        Sound_StopSong(0x202);
    component->flags |= 1;
    /* Callers ignore the unspecified ABI result. */
}
