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
extern Manager *gUnk_020000A4;
int Script_GetKeywordValue(int, int);
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
void *ScriptProc_Start(int, int);
int sub_08215CB8(void *, int, void *, int, int);
int TileAnimation_Initialize(Manager *p)
{
    int i, resource, mode;
    Channel *q;
    void *data;
    gUnk_020000A4 = p;
    p->count = Script_GetKeywordValue('s', 32);
    p->tiles = Heap_Alloc(p->count * 32);
    if (p->tiles) {
        Memory_Clear(p->tiles, p->count * 32);
        q = p->channels;
        {
            int zero = 0;
            i = 7;
            do {
                q->active = zero;
                q->remove = zero;
                q->count = zero;
                q->id = zero;
                i--;
                q++;
            } while (i >= 0);
        }
        resource = Script_GetKeywordValue('p', 0);
        mode = Script_GetKeywordValue('m', 0);
        data = ScriptProc_Start(0xceef, (u16)resource);
        if (data && sub_08215CB8(data, mode, p->tiles, 0, p->count) >= 0)
            return 0;
    }
    return -1;
}
