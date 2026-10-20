#include "global.h"
#include "scene_actor_reset.h"
void Sound_PlaySong(u32);
struct AxeFadeSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6;
};
static inline u8 TakeEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}

void Axe_UpdateFadeOut(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 entered = TakeEntry(actor);
    struct AxeFadeSprite *sprite = actor->drawable;
    s32 previous;
    if (entered)
        sprite->flags |= 2;
    previous = sprite->field6;
    sprite->field6 = previous - 6;
    if (previous < ((struct AxeFadeSprite *)actor->drawable)->field6)
        Sound_PlaySong(0x169);
    if (frame > 0x2B) {
        ((struct AxeFadeSprite *)actor->drawable)->field6 = 0;
        actor->field2B0 = 1;
    }
}
