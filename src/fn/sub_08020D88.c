#include "global.h"
#include "scene_event_dispatch.h"
typedef struct {
    u16 id;
    u8 kind, flags;
    u16 unknown4;
    u8 count[2];
    void *messages[2][4];
    u8 unknown28[8];
    void *next;
} Receiver;
typedef struct {
    u8 task[0x18];
    u16 scene, step, queued;
    u8 advance, changed;
    u16 onFinish;
    u8 scriptCount, active, buffer;
    u8 unknown25[3];
    u16 ids[16], scripts[16];
    int delay, unknown6c, signal;
    Receiver *receivers;
    Receiver self;
} Controller;

int SceneMessages_FindScript(int id)
{
    Controller *p = gSceneEventDispatchState;
    int i;
    if (p)
        goto scan;
    goto missing;
found:
    return p->scripts[i];
scan:
    for (i = 0; i < p->scriptCount; i++)
        if (p->ids[i] == id)
            goto found;
missing:
    return 0;
}
