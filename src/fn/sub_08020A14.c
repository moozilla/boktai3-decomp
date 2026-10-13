#include "global.h"
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    void *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;
void SceneMessages_SwapBuffers(u8 *p)
{
    Entry *entry = *(Entry **)(p + 0x74);
    while (entry) {
        Entry *next = entry->next;
        int i;
        entry->count[p[0x24]] = 0;
        for (i = 0; i < 4; i++)
            entry->messages[p[0x24]][i] = 0;
        entry = next;
    }
    p[0x24] = 1 - p[0x24];
}
