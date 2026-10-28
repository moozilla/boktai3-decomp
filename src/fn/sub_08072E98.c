// MATCHING_CONCESSION: register-pinning
#include "scene_actor_contact_list.h"
#include "player.h"
struct SceneActorContactObjectView {
    u8 unknown0[8];
    u8 playerSlot;
};
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
        struct Player *input = player;

        struct Player *player = input;
        u32 offset = 0x538;
        u32 value;
        u32 result;
        value = *(u16 *)((u32)input + offset);
        if (value) {
            offset -= 4;
            {
                u16 *field = (void *)((u32)input + offset);
                value = *field;
            }
            if (value == condition) {
                register u32 one asm("r1") = 1;
                asm("" : : "r"(one));
                goto success;
            }
        }
        if (!condition) {
            if (player) {
                u32 fieldOffset = 0x448;
                u32 mask;
                u32 *field;
                field = (void *)((u32)player + fieldOffset);
                mask = 4;
                if (*field & mask) {
                    register u32 one asm("r1") = 1;
                    asm("" : : "r"(one));
                    goto success;
                }
            }
            goto fail;
        }
        if (condition == 1 && player) {
            u32 fieldOffset = 0x448;
            u32 mask;
            u32 *field;
            u32 value;
            field = (void *)((u32)player + fieldOffset);
            mask = 0x40;
            value = *field & mask;
            result = 1;
            if (value)
                goto finalResult;
        }
    fail:
        result = 0;
    finalResult:
        if (!result)
            goto failure;
        goto success;
    }
    if (mode != 1 || !(player->statusFlags & condition))
        goto failure;
success:
    return 1;
failure:
    return 0;
}
