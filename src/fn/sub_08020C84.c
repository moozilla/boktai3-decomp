#include "global.h"
extern void *gUnk_0200047C;
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int SceneMessages_Initialize(void *, int);
void Task_Kill(void *);
void SceneMessages_Update(void);
void sub_08020C28(void);
void *SceneMessages_Create(int id)
{
    void *p;
    if (gUnk_0200047C)
        return 0;
    p = Task_Create(2, 0xac);
    if (p) {
        Task_SetCallbacks(p, SceneMessages_Update, sub_08020C28);
        if (SceneMessages_Initialize(p, id) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
