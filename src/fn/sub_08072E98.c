#include "scene_actor_contact_list.h"
#include "player.h"
struct SceneActorContactObjectView {
    u8 unknown0[8];
    u8 playerSlot;
};
static inline u8 TestCondition(struct Player *input, u32 condition)
{
    struct Player *player = input;
    u32 result;
    if (input->active && input->sceneMode == condition) {
        result = 1;
        goto done;
    }
    if (!condition) {
        if (player) {
            u32 *field = &player->actionFlags;
            u32 mask = 4;
            if (*field & mask) {
                result = 1;
                goto done;
            }
        }
        goto fail;
    }
    if (condition == 1 && player) {
        u32 *field = &player->actionFlags;
        u32 mask = 0x40;
        u32 value = *field & mask;
        result = 1;
        if (value)
            goto done;
    }
fail:
    result = 0;
done:
    return result;
}
u8 SceneActorContacts_TestPlayerCondition(struct SceneActorContactObjectView *object, u32 mode,
                                          u32 condition)
{
    u32 slot = 0;
    struct Player *player;
    switch (object->playerSlot) {
        case 0:
            break;
        case 1:
            slot = 1;
            break;
        case 2:
            slot = 2;
            break;
        case 3:
            slot = 3;
            break;
        default:
            return 0;
    }
    player = gPlayers[slot];
    if (!player)
        return 0;
    if (!mode) {
        if (!TestCondition(player, condition))
            return 0;
        return 1;
    }
    if (mode != 1 || !(player->statusFlags & condition))
        goto failure;
    return 1;
failure:
    return 0;
}
