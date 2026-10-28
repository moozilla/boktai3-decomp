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

int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int Script_GetPc(void);
int SceneMessages_ScriptStart(void)
{
    Controller *p = gSceneEventDispatchState;
    Receiver *self;
    u8 *countPtr;
    int countA;
    int countB;
    int i;
    int hasIds;
    if (!p || p->active == 1)
        return -1;
    p->active = 1;
    p->scene = Script_GetKeywordValue(0x64, -1);
    p->step = Script_GetKeywordValue(0x63, -1) - 1;
    p->queued = 0;
    p->advance = 1;
    p->changed = 0;
    p->onFinish = Script_GetKeywordValue(0x65, 0);
    p->scriptCount = 0;
    countA = 0;
    countB = 0;
    hasIds = Script_SeekToKeyword(0x70);
    countPtr = &p->scriptCount;
    self = &p->self;
    if (hasIds) {
        for (i = 0; i < 16 && Script_GetPc(); i++) {
            p->ids[countA] = Script_GetValue();
            countA++;
        }
    }
    if (Script_SeekToKeyword(0x50)) {
        for (i = 0; i < 16 && Script_GetPc(); i++) {
            p->scripts[countB] = Script_GetValue();
            countB++;
        }
    }
    if (countA > 0 || countB > 0)
        *countPtr = countA;
    p->unknown6c = 0;
    self->count[0] = 0;
    self->count[1] = 0;
    self->flags = 0;
    return 0;
}
