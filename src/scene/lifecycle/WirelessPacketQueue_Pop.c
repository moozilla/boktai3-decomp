#include "global.h"
struct WirelessPacketQueue {
    u8 empty, readIndex, writeIndex, count;
    u8 packets[32][16];
};
u32 WirelessPacketQueue_Pop(struct WirelessPacketQueue *queue, void *out)
{
    u32 zero;
    if (queue->empty)
        return 0;
    *(vu16 *)0x04000208 = 0;
    CpuSet(queue->packets[queue->readIndex], out, 0x04000004);
    zero = 0;
    CpuSet(&zero, queue->packets[queue->readIndex], 0x05000004);
    queue->count--;
    queue->readIndex = (queue->readIndex + 1) & 31;
    if (queue->readIndex == queue->writeIndex)
        queue->empty = 1;
    *(vu16 *)0x04000208 = 1;
    return 1;
}
