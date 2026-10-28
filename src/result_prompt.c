#include "result_prompt.h"

int ResultPrompt_Update(struct ResultPrompt *p)
{
    if (p->result < 120)
        gSceneDialogActive = 1;
    if (p->result < 0) {
        if (p->timer == 60) {
            p->result = Save_WriteAlternateSlotAndSelect();
            if (p->result == 1) {
                TextWindow_Refresh(p->window, 14);
                Sound_PlaySong(686);
            } else {
                TextWindow_Refresh(p->window, 15);
                Sound_PlaySong(402);
            }
        }
    }
    if ((p->result >= 0 && p->timer > 119 && (gUnk_03005260[1] & 1)) || p->timer > 299) {
        Hud_SetMode(7);
        if (p->script) {
            int value = p->result;
            struct ResultPromptArgs args;
            args.count = 1;
            args.values = &value;
            Script_Call(p->script, &args);
            p->script = 0;
        }
        Task_Kill(p);
    }
    p->timer++;
    return 0;
}

u32 ResultPrompt_Destroy(struct ResultPrompt *p)
{
    TextWindow_Destroy(p->window);
    return 0;
}

s32 ResultPrompt_Init(struct ResultPrompt *p)
{
    s32 r;
    Hud_SetMode(3);
    p->script = Script_GetKeywordValue(0x65, 0);
    p->timer = 0;
    r = -1;
    p->result = r;
    if (Script_SeekToKeyword(0x73)) {
        p->message = (const u8 *)Script_GetTextSourcePc();
        if (p->message != 0) {
            p->window = TextWindow_Create(1, 0xd, 0x1c, 6);
            if (p->window >= 0) {
                TextWindow_SetScript(p->window, p->message);
                TextWindow_Refresh(p->window, 0xb);
                TextWindow_StartPlayback(p->window);
                return 0;
            }
        }
    }
    return r;
}

u8 *ResultPrompt_Create(void)
{
    u8 *p;
    p = Task_Create(11, 0x2c);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))ResultPrompt_Update,
                          (void (*)(void *))ResultPrompt_Destroy);
        if (ResultPrompt_Init((struct ResultPrompt *)p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
