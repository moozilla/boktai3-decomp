#include "scene_actor_placement.h"
void SceneActorPlacement_PlayCategoryResponseSound(struct SceneActorRegistryActor *argument)
{
    struct SceneActorPlacementView *actor = (void *)argument;
    u32 mask = 0x400000;
    u32 *flags = &actor->flags2D0;
    if (mask & *flags) {
        *flags &= ~0x400000;
        return;
    }
    {
        unsigned long long soundFlags = actor->flags2E6;
        u32 soundMask = 4;
        if (SceneActorPlacement_TestMask(soundMask, soundFlags)) {
            return;
        }
    }
    switch (actor->category) {
        case 4:
            Sound_PlaySong(212);
            break;
        case 1:
            Sound_PlaySong(253);
            break;
        case 3:
            SceneActorPlacement_PlayVariantEventSound(actor, 1);
            break;
        case 2:
            Sound_PlaySong(274);
            break;
        case 9:
            Sound_PlaySong(412);
            break;
        case 18:
            Sound_PlaySong(264);
            break;
        case 22:
            Sound_PlaySong(959);
            break;
        case 5:
        case 6:
        case 7:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 17:
        case 20:
        case 21:
        case 23:
        case 24:
            Sound_PlaySong(318);
            break;
        case 19:
            Sound_PlaySong(265);
            break;
        case 25:
            Sound_PlaySong(459);
            break;
    }
}
