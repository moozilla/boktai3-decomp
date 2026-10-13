// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative peer-panel layout; clear its flags and the displayed name.
struct PeerPanel {
    u8 active;
    u8 pad1[3];
    u32 flags;
    u8 pad8[0x228];
    u8 name[16];
};

void *sub_08037DCC(int peer);
void sub_08219EBC(void *destination, const void *source, int size);
void sub_08033924(int window, int style);

int sub_08037540(void *owner, struct PeerPanel *p, int peer)
{
    void *name = sub_08037DCC(peer);
    u8 *destination;
    register int i asm("r1");
    int offset;
    register int zero asm("r2");

    p->flags = 0;
    zero = 0;
    asm("" : "+r"(zero));
    i = 15;
    offset = 0x23F;
    destination = (u8 *)p + offset;
    do {
        *destination-- = zero;
    } while (--i >= 0);
    sub_08219EBC(name, p->name, 16);
    {
        register int index asm("r1") = peer << 2;
        register int offset asm("r2") = 0x4E4;
        register u8 *window asm("r0");
        asm("" : "+r"(index), "+r"(offset));
        window = (u8 *)owner + offset;
        asm("" : "+r"(window));
        window += index;
        {
            register int id asm("r0") = *(int *)window;
            asm("" : "+r"(id));
            sub_08033924(id, peer + 3);
        }
    }
}
