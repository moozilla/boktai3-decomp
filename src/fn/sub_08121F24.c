#include "global.h"

extern u8 *gUnk_02000138;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneSpriteBurst_InitManager(u8 *);
void Task_Kill(u8 *);
void SpriteGroup_UpdateEntries(void);
void SpriteGroup_RemoveSprites(void);

u8 *SceneSpriteBurst_EnsureSingleton(void)
{
    u8 *p;
    if (gUnk_02000138 != 0)
        return gUnk_02000138;
    p = Task_Create(0xa, 0x27c);
    if (p != 0) {
        Task_SetCallbacks(p, SpriteGroup_UpdateEntries, SpriteGroup_RemoveSprites);
        if (SceneSpriteBurst_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
