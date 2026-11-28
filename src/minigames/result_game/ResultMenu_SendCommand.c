#include "minigames/result_game/result_tutorial.h"
s32 SceneMessages_Send(struct SceneEventCommand *);

int ResultMenu_SendCommand(struct ResultMenu *menu, int command, int count, u16 *args)
{
    struct SceneEventCommand *packet = &menu->command;
    packet->id = menu->receiverId;
    packet->receiverKind = menu->receiverKind;
    packet->trigger = 0;
    packet->unknown4 = 0;
    packet->kind = command;
    packet->argumentCount = count;
    if (args && count > 0) {
        u16 *src = args;
        u16 *out;
        int n;
        out = (u16 *)&menu->command.value;
        n = count;
        do {
            *out++ = *src++;
        } while (--n);
    }
    SceneMessages_Send(packet);
    return 0;
}
