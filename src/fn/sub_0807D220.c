#include "contact_actor.h"
#include "player.h"
#include "scene_actor_contact_list.h"
#include "ram.h"
struct RetargetActorView {
    u8 unknown0[0xDC];
    void *targetSource;
    u8 unknownE0[0x2D4 - 0xE0];
    u16 flags2D4;
    u8 unknown2D6[0x45C - 0x2D6];
    void *ownedTarget;
};
void SceneActorAttachment_RetargetNearestPlayer(struct RetargetActorView *input,
                                                const struct ContactPosition *inputOrigin)
{
    struct RetargetActorView *actor = input;
    const struct ContactPosition *origin = inputOrigin;
    u32 nearest = 0;
    s32 bestDistance = 0x7fffffff;
    /* Only the three named halfwords are read. Padding remains unspecified. */
    struct ContactPosition difference;
    s32 initialCount;
    initialCount = gPlayerCount;
    if ((s32)nearest < initialCount) {
        struct ContactPosition *delta = &difference;
        struct Player **slot = gPlayers;
        s32 count = initialCount;
        do {
            struct Player *player = *slot;
            if (player) {
                const struct SceneVec *position = &player->position;
                s32 x;
                s32 z;
                s32 xSquare;
                s32 zSquare;
                s32 distance;
                delta->x = (u16)player->position.x - (u16)origin->x;
                delta->y = (u16)position->y - (u16)origin->y;
                delta->z = (u16)position->z - (u16)origin->z;
                x = delta->x;
                xSquare = x * x;
                z = delta->z;
                zSquare = z * z;
                {
                    s32 copy = zSquare;
                    distance = (s32)((u32)xSquare + (u32)copy);
                }
                if (bestDistance > distance) {
                    bestDistance = distance;
                    nearest = (u32)*slot + 0x24;
                }
            }
            slot++;
            count--;
        } while (count != 0);
    }
    if (nearest) {
        u32 mask = 0x1000;
        u16 *flags = &actor->flags2D4;
        u32 value = *flags;
        if (value & mask) {
            void **owned = &actor->ownedTarget;
            SceneActorContacts_ReleaseNode(*owned);
            {
                u32 updated = *flags & ~mask;
                u32 zero = 0;
                *flags = updated;
                *owned = (void *)zero;
            }
        }
        actor->targetSource = (void *)nearest;
    }
}
