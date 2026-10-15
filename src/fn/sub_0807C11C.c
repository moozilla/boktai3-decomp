#include "engine.h"
#include "scene_actor_attachment.h"
void SceneActorEffect_PlayCategorySound(u8 event, u8 category, u8 variant)
{
    switch (category) {
        case 4:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(240);
                    break;
                case 1:
                    Sound_PlaySong(239);
                    break;
                case 2:
                    Sound_PlaySong(241);
                    break;
            }
            break;
        case 1:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(259);
                    break;
                case 1:
                    Sound_PlaySong(258);
                    break;
                case 2:
                    Sound_PlaySong(260);
                    break;
            }
            break;
        case 3:
            switch (event) {
                case 0:
                case 3:
                    switch (variant) {
                        case 0:
                        case 1:
                        case 3:
                        case 5:
                            Sound_PlaySong(390);
                            break;
                    }
                    break;
                case 1:
                    switch (variant) {
                        case 0:
                        case 1:
                        case 3:
                        case 5:
                            Sound_PlaySong(389);
                            break;
                    }
                    break;
                case 2:
                    switch (variant) {
                        case 0:
                        case 1:
                        case 3:
                        case 5:
                            Sound_PlaySong(391);
                            break;
                    }
                    break;
            }
            break;
        case 21:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(932);
                    break;
                case 1:
                    Sound_PlaySong(931);
                    break;
                case 2:
                    Sound_PlaySong(315);
                    break;
            }
            break;
        case 10:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(625);
                    break;
                case 1:
                    Sound_PlaySong(625);
                    break;
                case 2:
                    Sound_PlaySong(626);
                    break;
            }
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(314);
                    break;
                case 1:
                    Sound_PlaySong(313);
                    break;
                case 2:
                    Sound_PlaySong(315);
                    break;
            }
            break;
        case 23:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(314);
                    break;
                case 1:
                    Sound_PlaySong(313);
                    break;
                case 2:
                    Sound_PlaySong(315);
                    break;
            }
            break;
        case 25:
            switch (event) {
                case 0:
                case 3:
                    Sound_PlaySong(456);
                    break;
                case 1:
                    Sound_PlaySong(455);
                    break;
                case 2:
                    Sound_PlaySong(457);
                    break;
            }
            break;
    }
}
