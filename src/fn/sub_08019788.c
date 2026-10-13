#include "global.h"
typedef struct {
    u16 id, time;
    u8 active, count, frame, frames, unpaused, dirty, remove, pad;
    u16 source[8], dest[8], duration[8];
} Channel;
typedef struct {
    u8 pad[24];
    u16 count, unused;
    Channel channels[8];
    void *tiles;
} Manager;

Manager *sub_080195DC(void);
void TileAnimation_Upload(void)
{
    Manager *p = sub_080195DC();
    Channel *q;
    int i;
    if (p) {
        q = p->channels;
        {
            void **tiles = &p->tiles;
            volatile u32 *dma = (volatile u32 *)0x040000d4;
            i = 7;
            do {
                if (q->active && q->dirty) {
                    u32 src = (u32)*tiles + q->source[q->frame] * 32;
                    u32 dst = 0x06000000 + q->dest[q->frame] * 32;
                    dma[0] = src;
                    dma[1] = dst;
                    dma[2] = (q->count * 8) | 0x84000000;
                    dma[2];
                    q->dirty = 0;
                }
                i--;
                q++;
            } while (i >= 0);
        }
    }
}
