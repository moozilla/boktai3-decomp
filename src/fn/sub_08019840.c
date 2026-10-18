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

Manager *TileAnimation_GetTask(void);
Channel *TileAnimation_FindFreeChannel(Manager *);
Channel *TileAnimation_FindChannel(Manager *, int);
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetPc(void);
int Script_GetValue(int);
void TileAnimation_Configure(void)
{
    Manager *p = TileAnimation_GetTask();
    Channel *q;
    int i, v;
    u16 *out;
    if (!p)
        return;
    q = TileAnimation_FindFreeChannel(p);
    if (!q)
        return;
    q->id = Script_GetKeywordValue('c', 0);
    if (q->id && TileAnimation_FindChannel(p, q->id))
        return;
    q->count = Script_GetKeywordValue('s', 1);
    q->frame = Script_GetKeywordValue('S', 0);
    q->frames = Script_GetKeywordValue('a', 1);
    q->unpaused = Script_GetKeywordValue('p', 0);
    q->dirty = 1;
    q->time = 0;
    if (q->count > p->count)
        q->count = p->count;
    if (!q->count)
        q->count = 1;
    if (q->frames > 8)
        q->frames = 8;
    else if (!q->frames)
        q->frames = 1;
    if (q->frame >= q->frames)
        q->frame = q->frames - 1;

    if (Script_SeekToKeyword('R')) {

        i = 0;
        if (i < q->frames) {
            out = q->source;
            do {
                v = Script_GetPc();
                if (v)
                    v = Script_GetValue(v);
                *out = v;
                out++;
                i++;
            } while (i < q->frames);
        }
    } else {

        for (i = 0; i < q->frames; i++)
            q->source[i] = 0;
    }

    if (Script_SeekToKeyword('d')) {

        i = 0;
        if (i < q->frames) {
            out = q->dest;
            do {
                v = Script_GetPc();
                if (v)
                    v = Script_GetValue(v);
                *out = v;
                out++;
                i++;
            } while (i < q->frames);
        }
    } else {

        for (i = 0; i < q->frames; i++)
            q->dest[i] = 0;
    }

    if (Script_SeekToKeyword('t')) {

        i = 0;
        if (i < q->frames) {
            out = q->duration;
            do {
                v = Script_GetPc();
                if (v)
                    v = Script_GetValue(v);
                *out = v;
                out++;
                i++;
            } while (i < q->frames);
        }
    } else {

        for (i = 0; i < q->frames; i++)
            q->duration[i] = 0;
    }
    q->remove = 0;
    q->active = 1;
}
