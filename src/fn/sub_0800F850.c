#include "global.h"
typedef struct {
    u8 task[24];
    int text;
    u32 script;
    int timer, result;
    void *message;
} ResultPrompt;
extern int gUnk_03005408;
extern u16 gUnk_03005260[];
u8 sub_0822BE20(void);
void TextWindow_Refresh(int, int);
void Sound_PlaySong(int);
void sub_08177DEC(int);
void Script_Call(u32, void *);
void Task_Kill(void *);
int ResultPrompt_Update(ResultPrompt *p)
{
    if (p->result < 120)
        gUnk_03005408 = 1;
    if (p->result < 0) {
        if (p->timer == 60) {
            p->result = sub_0822BE20();
            if (p->result == 1) {
                TextWindow_Refresh(p->text, 14);
                Sound_PlaySong(686);
            } else {
                TextWindow_Refresh(p->text, 15);
                Sound_PlaySong(402);
            }
        }
    }
    if ((p->result >= 0 && p->timer > 119 && (gUnk_03005260[1] & 1)) || p->timer > 299) {
        sub_08177DEC(7);
        if (p->script) {
            int value = p->result;
            struct {
                u32 count : 16;
                u32 pad : 16;
                int *values;
            } args;
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
