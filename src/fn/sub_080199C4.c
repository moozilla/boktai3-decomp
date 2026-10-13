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
Channel *sub_08019818(Manager *, int);
int Script_GetKeywordValue(int, int);
void TileAnimation_Stop(void)
{
    Manager *p = sub_080195DC();
    Channel *q;
    int id, frame;
    if (p) {
        id = Script_GetKeywordValue('c', 0);
        if (id) {
            q = sub_08019818(p, id);
            if (q) {
                q->remove = 1;
                frame = Script_GetKeywordValue('i', -1);
                if (frame >= 0) {
                    q->frame = frame;
                    if (q->frame >= q->frames)
                        q->frame = q->frames - 1;
                    q->dirty = 1;
                }
            }
        }
    }
}
