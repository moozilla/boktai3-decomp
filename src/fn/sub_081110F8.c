#include "global.h"
extern u8 *gUnk_020004B8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 TilemapPanel_Initialize(u8 *);
void Task_Kill(u8 *);
void SceneTilemap_Update(void);
void SceneTilemap_Destroy(void);
u8 *SceneTilemap_CreateFromScript(void)
{
    u8 *p;
    if (gUnk_020004B8 != 0)
        return gUnk_020004B8;
    p = Task_Create(2, 0x60);
    if (p != 0) {
        Task_SetCallbacks(p, SceneTilemap_Update, SceneTilemap_Destroy);
        if (TilemapPanel_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
