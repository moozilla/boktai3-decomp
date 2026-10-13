#include "result_prompt.h"

int ResultPrompt_Update(struct ResultPrompt *p)
{
    if (p->result < 120)
        gUnk_03005408 = 1;
    if (p->result < 0) {
        if (p->timer == 60) {
            p->result = sub_0822BE20();
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
        sub_08177DEC(7);
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

u32 sub_0800F914(struct ResultPrompt *p)
{
    sub_08033778(p->window);
    return 0;
}

s32 sub_0800F924(struct ResultPrompt *p)
{
    s32 r;
    sub_08177DEC(3);
    p->script = Script_GetKeywordValue(0x65, 0);
    p->timer = 0;
    r = -1;
    p->result = r;
    if (Script_SeekToKeyword(0x73)) {
        p->message = (const u8 *)sub_08227E90();
        if (p->message != 0) {
            p->window = sub_08033690(1, 0xd, 0x1c, 6);
            if (p->window >= 0) {
                TextWindow_SetScript(p->window, p->message);
                TextWindow_Refresh(p->window, 0xb);
                sub_080337FC(p->window);
                return 0;
            }
        }
    }
    return r;
}

u8 *sub_0800F988(void)
{
    u8 *p;
    p = Task_Create(11, 0x2c);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))ResultPrompt_Update, (void (*)(void *))sub_0800F914);
        if (sub_0800F924((struct ResultPrompt *)p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
