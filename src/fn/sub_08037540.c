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
    int i;
    int offset;
    int zero;

    p->flags = 0;
    zero = 0;
    i = 15;
    offset = 0x23F;
    destination = (u8 *)p + offset;
    do {
        *destination-- = zero;
    } while (--i >= 0);
    sub_08219EBC(name, p->name, 16);
    {
        int index = peer << 2;
        int offset = 0x4E4;
        u8 *window;
        window = (u8 *)owner + offset;
        window += index;
        {
            int id = *(int *)window;
            sub_08033924(id, peer + 3);
        }
    }
}
