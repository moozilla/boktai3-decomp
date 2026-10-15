// MATCHING_CONCESSION: register-pinning
#include "scene_actor_contact_list.h"
#include "player.h"
#define SceneActorContacts_TestPlayerCondition sub_08072E98
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
    player = gUnk_02000580[slot];
    if (!player)
        return 0;
    if (!mode) {
        struct Player *input = player;

        struct Player *player = input;
        register u32 offset asm("r4") = 0x538;
        u32 value;
        register u32 result asm("r1");
        value = *(u16 *)((u32)input + offset);
        if (value) {
            offset -= 4;
            {
                register u16 *field asm("r0") = (void *)((u32)input + offset);
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
                register u32 fieldOffset asm("r1") = 0x448;
                u32 mask;
                register u32 *field asm("r0");
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
            register u32 fieldOffset asm("r4") = 0x448;
            u32 mask;
            register u32 *field asm("r0");
            u32 value;
            asm("" : "+r"(fieldOffset));
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
