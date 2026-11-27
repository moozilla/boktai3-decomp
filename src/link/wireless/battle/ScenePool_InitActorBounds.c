#include "global.h"
#include "link/wireless/scene_pool.h"
struct Blob {
    u32 words[11];
};
struct P {
    u8 pad[0x84];
    struct Blob blob;
};
struct Player {
    u8 pad[0x9c];
    struct Blob blob;
};
void ActorStats_ConfigureScaledParameters(void *, int, int, int, int, int, int);
void ScenePool_InitActorBounds(struct P *p, struct ScenePoolEntry *item)
{
    struct Player *player = (*(struct Player **)gParticipantActors);
    int okay = 0;
    int x = 0;
    int z = 0;
    if (player) {
        switch (item->kind) {
            case 0:
                // concession-ok(duplicate-branches): the ROM keeps the mode test and both identical arms, so the original wrote separate per-mode values that happen to be equal
                if (!item->mode) {
                    okay = 1;
                    z = 1024;
                    x = z;
                } else {
                    okay = 1;
                    z = 1024;
                    x = z;
                }
                break;
            case 1:
                // concession-ok(duplicate-branches): the ROM keeps the mode test and both identical arms, so the original wrote separate per-mode values that happen to be equal
                if (!item->mode) {
                    okay = 1;
                    z = 1024;
                    x = z;
                } else {
                    okay = 1;
                    z = 1024;
                    x = z;
                }
                break;
            case 2:
                okay = 1;
                x = 2048;
                z = 1024;
                break;
            case 3:
                okay = 1;
                z = 2048;
                x = z;
                break;
        }
    }
    if (okay) {
        p->blob = player->blob;
        p->blob.words[4] = 0x1000;
        p->blob.words[5] += x;
        p->blob.words[9] += z;
    } else
        ActorStats_ConfigureScaledParameters(p, 0x1000, 0x1000, -0x1000, 0x2000, okay, 0x4000);
}
