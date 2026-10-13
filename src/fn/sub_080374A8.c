// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative peer-panel layout: packet names are decoded into the local name.
struct PeerPanel {
    u8 active;
    u8 pad1[3];
    u32 flags;
    u8 pad8[0x228];
    u8 name[16];
};

void *sub_08037DCC(int peer);
void sub_08219EBC(void *destination, const void *source, int size);
void sub_08036CF0(const u8 *source, u8 *destination, int count);
void sub_08033924(int window, int style);
void CpuSet(const void *source, void *destination, u32 control);

int sub_080374A8(void *ownerArg, struct PeerPanel *panel, int peerArg, int playerArg, const u8 *packetArg)
{
    register u8 *owner asm("r8") = ownerArg;
    register struct PeerPanel *p asm("r6") = panel;
    register int peer asm("r9") = peerArg;
    register int player asm("r10") = playerArg;
    register const u8 *packet asm("r5") = packetArg;
    void *name;
    u32 zero;
    u8 *scratch;
    u8 *destination;
    asm("" : "+r"(owner), "+r"(p), "+r"(peer), "+r"(player), "+r"(packet));

    name = sub_08037DCC(peer);
    p->flags = 0;
    zero = 0;
    scratch = owner + 0x4F4;
    CpuSet(&zero, scratch, 0x05000001);
    packet += 4 + player * 10;
    sub_08219EBC(scratch, packet, 16);
    destination = p->name;
    sub_08036CF0(scratch, destination, 8);
    sub_08219EBC(name, destination, 16);
    {
        register int index asm("r0") = peer << 2;
        register int offset asm("r1") = 0x4E4;
        int window;
        asm("" : "+r"(index), "+r"(offset));
        owner += offset;
        owner += index;
        asm("" : "+r"(owner));
        {
            register u8 *address asm("r1") = owner;
            asm("" : "+r"(address));
            window = *(int *)address;
        }
        peer += 3;
        sub_08033924(window, peer);
    }
}
