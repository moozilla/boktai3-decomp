#include "global.h"
struct Packet {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[8];
};
void SceneMessages_Send(struct Packet *);
void sub_0815DBF4(u8 *p, u8 command, s32 count, u16 *args)
{
    struct Packet *packet = (struct Packet *)(p + 0xb0c);
    s32 i;
    packet->id = *(u32 *)(p + 0x24);
    packet->kind = 2;
    packet->zero = 0;
    packet->reserved = 0;
    packet->command = command;
    packet->count = count;
    if (args)
        for (i = 0; i < count; i++)
            packet->args[i] = args[i];
    SceneMessages_Send(packet);
}
