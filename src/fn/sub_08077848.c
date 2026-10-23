#include "scene_actor_placement.h"
void SceneActorPlacement_PlayVariantEventSound(struct SceneActorPlacementView *actor, u32 event)
{
    s32 variant = actor->paletteOffset;
    switch (event) {
        case 2:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySongChecked(392);
                    break;
            }
            break;
        case 3:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySongChecked(391);
                    break;
            }
            break;
        case 1:
            if (!actor->field2E4) {
                Sound_PlaySong(529);
            } else {
                unsigned long long soundFlags = actor->flags2E6;
                u32 soundMask = 4;
                if (!SceneActorPlacement_TestMask(soundMask, soundFlags)) {
                    switch ((s32)actor->paletteOffset) {
                        case 0:
                        case 1:
                        case 3:
                        case 5:
                            Sound_PlaySong(318);
                            break;
                    }
                }
            }
            break;
        case 4:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySong(393);
                    break;
            }
            break;
        case 5:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySong(394);
                    break;
            }
            break;
        case 6:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySong(395);
                    break;
            }
            break;
        case 0:
            switch (variant) {
                case 0:
                case 1:
                case 3:
                case 5:
                    Sound_PlaySongChecked(398);
                    break;
            }
            break;
    }
}
