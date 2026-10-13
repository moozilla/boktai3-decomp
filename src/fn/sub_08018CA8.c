#include "global.h"
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int OrbitingParticles_Update(void *);
int sub_080189A8(void *);
int OrbitingParticles_Initialize(void *, void *, int, int, int, int);
void Task_Kill(void *);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int Script_GetKeywordValue(int, int);
struct Vec {
    signed x : 16;
    signed y : 16;
    signed z : 16;
};
void *OrbitingParticles_CreateFromScript(void)
{
    void *p = Task_Create(11, 436);
    struct Vec pos;
    int radius, duration, count, type;
    if (p) {
        if (Script_SeekToKeyword('p')) {
            pos.x = Script_GetValue();
            pos.y = Script_GetValue();
            pos.z = Script_GetValue();
        } else {
            pos.x = 0;
            pos.y = 0;
            pos.z = 0;
        }
        radius = Script_GetKeywordValue('d', 256);
        duration = Script_GetKeywordValue('t', 256);
        count = Script_GetKeywordValue('n', 8);
        type = Script_GetKeywordValue('T', 0);
        Task_SetCallbacks(p, OrbitingParticles_Update, sub_080189A8);
        if (OrbitingParticles_Initialize(p, &pos, radius, duration, count, type) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
