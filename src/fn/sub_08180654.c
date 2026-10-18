#define SpriteComponent_Suspend sub_08180654
#include "global.h"
struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
    u32 flags;
};
void Sound_StopSong(u32);
u32 sub_08180654(struct SpriteComponent *component)
{
    u32 flags = component->flags;
    u32 mask = 1;
    u32 active = flags & mask;
    if (!active)
        Sound_StopSong(0x202);
    component->flags |= mask;
    /* Callers ignore the unspecified ABI result. */
}
