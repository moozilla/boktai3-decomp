#include "global.h"
typedef struct {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[1];
} Packet;
void sub_08020DD0(void *);
void sub_08029FB0(u8 *p)
{
    Packet *packet = (void *)(p + 0x914);
    if (*(int *)(p + 0x910) && p[0x1a8] != 1) {
        int zero = 0;
        p[0x1a8] = 1;
        packet->id = *(int *)(p + 0x910);
        packet->kind = 5;
        packet->zero = 0;
        packet->reserved = zero;
        packet->command = 92;
        packet->count = 0;
        sub_08020DD0(packet);
    }
}
