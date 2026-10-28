#include "global.h"
#include "engine.h"
#include "ram.h"
/* Message packet, as sent by SceneMessages_Send (see GridAttackActor_NotifyLinkedActorLatePhase). */
struct Packet {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[8];
    u8 pad18[0x28];
};
/* Two pages of up to eight queued packets; `page` selects the one being filled. */
struct PacketQueue {
    u8 pad0[0x28];
    u32 page;
    u32 counts[2];
    struct Packet packets[2][8];
};
void SceneMessages_Send(struct Packet *);
/* Script: fills the next packet of the current page (-n id, -k kind, -f, -a command,
   -p up to eight arguments), sends it and counts it. The bounds check indexes
   counts[] with the count itself, as the original does. */
s32 Packet_QueueFromScript(void)
{
    struct PacketQueue *queue = gSceneActorListManager;
    struct Packet *packet;
    if (!queue || queue->counts[queue->counts[queue->page]] > 7)
        return -1;
    packet = &queue->packets[queue->page][queue->counts[queue->page]];
    packet->id = Script_GetKeywordValue('n', 0);
    packet->kind = Script_GetKeywordValue('k', 0);
    packet->zero = 0;
    packet->reserved = Script_GetKeywordValue('f', 0);
    packet->command = Script_GetKeywordValue('a', 0);
    packet->count = 0;
    if (Script_SeekToKeyword('p')) {
        while (Script_GetPc()) {
            packet->args[packet->count++] = Script_GetValue();
            if (packet->count > 7)
                break;
        }
    }
    SceneMessages_Send(packet);
    queue->counts[queue->page]++;
    return 0;
}
