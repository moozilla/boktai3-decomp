#include "global.h"
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    void *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;
Entry *SceneMessages_FindReceiver(u8 *, int, int);
int SceneMessages_Enqueue(u8 *p, u8 *message)
{
    Entry *entry = SceneMessages_FindReceiver(p, *(u16 *)message, message[2]);
    int buffer = 1 - p[0x24];
    if (entry && entry->count[buffer] < 4) {
        entry->messages[buffer][entry->count[buffer]] = message;
        entry->count[buffer]++;
    }
}
