#include "scene_actor_placement.h"
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *argument)
{
    struct SceneActorPlacementView *actor = (void *)argument;
    u32 mask = 0x400000;
    u32 *flags = &actor->flags2D0;
    if (mask & *flags) {
        *flags &= ~0x400000;
        return;
    }
    switch (actor->category) {
        case 4:
            Sound_PlaySong(215);
            break;
        case 1:
            Sound_PlaySong(256);
            break;
        case 3:
            SceneActorPlacement_PlayVariantEventSound(actor, 0);
            break;
        case 2:
            Sound_PlaySong(275);
            break;
        case 5:
        case 6:
        case 7:
            Sound_PlaySong(940);
            break;
        case 9:
            Sound_PlaySong(981);
            break;
        case 17:
            Sound_PlaySong(228);
            break;
        case 18:
            Sound_PlaySong(232);
            break;
        case 19:
            Sound_PlaySong(248);
            break;
        case 21:
            Sound_PlaySong(930);
            break;
        case 22:
            Sound_PlaySong(960);
            break;
        case 20:
            Sound_PlaySong(965);
            break;
        case 23:
            if (SceneActorPlacement_ReadFlags16(actor, 0x4000))
                Sound_PlaySong(542);
            else
                Sound_PlaySong(970);
            break;
        case 14:
        case 24:
            Sound_PlaySong(471);
            break;
        case 25:
            Sound_PlaySong(458);
            break;
    }
}
