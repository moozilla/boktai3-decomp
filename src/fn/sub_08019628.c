#include "global.h"
#include "render.h"
typedef struct {
    u16 id, time;
    u8 active, count, frame, frames, unpaused, dirty, remove, pad;
    u16 source[8], dest[8], duration[8];
} Channel;
typedef struct {
    u8 pad[28];
    Channel channels[8];
    void *tiles;
} Manager;
void TileAnimation_Advance(Manager *, Channel *);
int TileAnimation_Update(Manager *p)
{
    Channel *q = p->channels;
    int i = 7;
    do {
        if (q->active) {
            if (q->remove) {
                q->active = 0;
                q->remove = 0;
            } else if (q->unpaused || !gEnginePauseFlags)
                TileAnimation_Advance(p, q);
        }
        i--;
        q++;
    } while (i >= 0);
    return 0;
}
