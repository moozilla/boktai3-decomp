#include "global.h"
#define SceneMenu_SetState Registry1745_SetState
#define gSceneMenuPeer gUnk_020004B4
#define SceneMenu_BeginTimedMessage sub_0810B324
struct SceneMenu {
    u8 pad0[0x30];
    u32 timer;
    u8 pad34[0x84];
    s32 window;
    u8 padBC[0x1A0];
    const u8 *script;
    u32 field260;
    u32 field264;
};
extern u8 *gUnk_020004B4;
u8 Registry1745_ConsumeStateEntry(struct SceneMenu *);
s32 Registry1745_CheckSessionReady(struct SceneMenu *);
void Registry1745_BeginShutdown(struct SceneMenu *, void (*)(void));
void Registry1745_SetState(struct SceneMenu *, u32, void (*)(void));
void sub_08109C80(void);
void sub_0810B3BC(void);
s32 sub_08033690(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
s32 sub_0810B324(struct SceneMenu *p)
{
    if (Registry1745_ConsumeStateEntry(p)) {
        p->field260 = 0;
        p->field264 = 0;
        if (gUnk_020004B4)
            gUnk_020004B4[0x52] = 1;
        p->window = sub_08033690(1, 6, 28, 6);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 8);
        TextWindow_StartPlayback(p->window);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (p->timer <= 59)
        Registry1745_SetState(p, 0x16, sub_0810B3BC);
    return 0;
}
