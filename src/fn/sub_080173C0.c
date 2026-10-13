#include "global.h"
extern void *gUnk_0200007C;
int Script_GetKeywordValue(int, int);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int ScanlineWave_Update(void *);
int sub_080172D8(void *);
int ScanlineWave_Initialize(void *, int);
void Task_Kill(void *);
void *ScanlineWave_Create(int id)
{
    void *p;
    if (gUnk_0200007C)
        return gUnk_0200007C;
    if (Script_GetKeywordValue('a', 0))
        p = Task_Create(11, 628);
    else
        p = Task_Create(9, 628);
    if (p) {
        Task_SetCallbacks(p, ScanlineWave_Update, sub_080172D8);
        if (ScanlineWave_Initialize(p, (u16)id) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
