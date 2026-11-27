#include "scene/actor/scene_event_types.h"
s32 SceneMessages_Send(struct SceneEventCommand *);
int ResultMenu_SendCommand(u8 *p, int command, int count, u16 *args)
{
    struct SceneEventCommand *packet = (struct SceneEventCommand *)(p + 0x7b0);
    packet->id = *(u16 *)(p + 0x7ac);
    packet->receiverKind = *(u16 *)(p + 0x7ae);
    packet->trigger = 0;
    packet->unknown4 = 0;
    packet->kind = command;
    packet->argumentCount = count;
    if (args && count > 0) {
        u16 *src = args;
        u16 *out;
        int n;
        out = (u16 *)(p + 0x7b8);
        n = count;
        do {
            *out++ = *src++;
        } while (--n);
    }
    SceneMessages_Send(packet);
    return 0;
}
