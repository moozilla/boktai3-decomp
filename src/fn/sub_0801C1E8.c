#include "global.h"
typedef struct {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[1];
} Packet;
void sub_08020DD0(void *);
int ResultMenu_SendCommand(u8 *p, int command, int count, u16 *args)
{
    Packet *packet = (Packet *)(p + 0x7b0);
    packet->id = *(u16 *)(p + 0x7ac);
    packet->kind = *(u16 *)(p + 0x7ae);
    packet->zero = 0;
    packet->reserved = 0;
    packet->command = command;
    packet->count = count;
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
    sub_08020DD0(packet);
    return 0;
}
