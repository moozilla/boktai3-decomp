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
Channel *sub_080197F8(Manager *);
Channel *sub_08019818(Manager *, int);
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetPc(void);
int Script_GetValue(int);
void TileAnimation_Configure(void)
{
    Manager *p = sub_080195DC();
    Channel *q;
    int i, v;
    u16 *out;
    if (!p)
        return;
    q = sub_080197F8(p);
    if (!q)
        return;
    q->id = sub_0821ABA8('c', 0);
    if (q->id && sub_08019818(p, q->id))
        return;
    q->count = sub_0821ABA8('s', 1);
    q->frame = sub_0821ABA8('S', 0);
    q->frames = sub_0821ABA8('a', 1);
    q->unpaused = sub_0821ABA8('p', 0);
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
