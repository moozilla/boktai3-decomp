#include "player_cue.h"
struct Packet {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[8];
};
void SceneMessages_Send(struct Packet *);
void sub_0815DBF4(struct Player *p, u8 command, s32 count, u16 *args)
{
    struct Packet *packet = (struct Packet *)p->unkB0C;
    s32 i;
    packet->id = PlayerCue_Messages(p)->receiverIdentifier;
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
