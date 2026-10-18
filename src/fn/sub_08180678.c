#define SpriteComponent_Resume sub_08180678
#include "global.h"
struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
    u32 flags;
};
void Sound_PlaySong(u32);
u32 sub_08180678(struct SpriteComponent *component)
{
    if (component->flags & 1)
        Sound_PlaySong(0x202);
    component->flags &= ~1;
    /* Callers must ignore the unspecified ABI result. */
}
