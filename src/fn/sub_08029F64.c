#include "global.h"
typedef struct {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[1];
} Packet;
void sub_08020DD0(void *);
void sub_08029F64(u8 *p)
{
    Packet *packet = (void *)(p + 0x914);
    if (*(int *)(p + 0x910) && p[0x1a8] != 0) {
        int zero = 0;
        p[0x1a8] = zero;
        packet->id = *(int *)(p + 0x910);
        packet->kind = 5;
        packet->zero = 0;
        packet->reserved = zero;
        packet->command = 1;
        packet->count = 1;
        packet->args[0] = 3;
        sub_08020DD0(packet);
    }
}
