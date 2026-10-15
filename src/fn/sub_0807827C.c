#include "scene_actor_behavior.h"
void SceneActorBehavior_PlayReactionSound(struct SceneActorBehaviorView *actor)
{
    u32 m = 6;
    if (actor->flags2D4 & m) {
        Sound_PlaySong(0x13e);
        return;
    }
    switch (actor->category) {
        case 4:
            Sound_PlaySong(0xd4);
            break;
        case 1:
            Sound_PlaySong(0xfd);
            break;
        case 3:
            switch (actor->paletteOffset) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySong(0x18d);
                    break;
            }
            break;
        case 0xe:
            Sound_PlaySong(0x1dc);
            break;
        case 0x19:
            Sound_PlaySong(0x1cb);
            break;
    }
}
