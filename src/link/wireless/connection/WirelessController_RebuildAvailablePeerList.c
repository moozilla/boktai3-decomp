#include "global.h"
#include "engine/ram.h"
#include "link/wireless/rfu_manager.h"
#include "lib/rfu/rfu_compat.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
struct Record {
    u8 pad0, mode, pad2[6], count, pad9[13], status, pad17[9];
};
void WirelessController_RebuildAvailablePeerList(struct Engine *p)
{
    int i;
    int left, right, limit;
    p->availablePeerCount = 0;
    i = 0;
    for (; i < ((struct Record *)gRfuLinkStatus)->count; i++) {
        if ((p->availableSlots >> i) & 1) {
            {
                struct Record *base = ((struct Record *)gRfuLinkStatus);
                int offset;
                offset = i << 5;
                base = (struct Record *)((u32)base + offset);
                if (base->status <= 3) {
                    p->availablePeerIndices[p->availablePeerCount] = i;
                    p->availablePeerCount++;
                }
            }
        }
    }
    left = p->peerScroll - 2;
    right = p->peerScroll + 3;
    if (left < 0) {
        i = left;
        do {
            i++;
        } while (i < 0);
    }
    i = left;
    if (i < 0)
        i = 0;
    goto test;
loop:
    i++;
test:
    limit = right;
    if (right > 8)
        limit = 8;
    if (i < limit)
        goto loop;
    if (right > 7 && i < right) {
        i = right - i;
        do {
            i--;
        } while (i);
    }
}
