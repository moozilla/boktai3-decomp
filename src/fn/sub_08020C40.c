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

int SceneMessages_RegisterReceiver(void *, int, int);
int SceneMessages_Initialize(Controller *p, int id)
{
    gSceneEventDispatchState = p;
    p->active = 0;
    p->scene = 0xffff;
    p->step = -1;
    p->queued = 0;
    p->advance = 0;
    p->changed = 0;
    p->onFinish = 0;
    p->scriptCount = 0;
    p->unknown6c = 0;
    p->receivers = 0;
    SceneMessages_RegisterReceiver(&p->self, id, 1);
    return 0;
}
