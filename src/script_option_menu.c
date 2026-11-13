#include "script_option_menu.h"

void OptionMenu_Update(struct OptionMenu *p)
{
    s32 zero = 0;
    struct ScriptUtilityArguments args;
    struct ScriptUtilityArguments *ap;
    args.count = 1;
    args.values = &zero;
    if (gPadState[1] & 0x40) {
        Sound_PlaySong(0xDC);
        p->selection--;
        if (p->selection < 0)
            p->selection = 7;
    }
    ap = &args;
    if (gPadState[1] & 0x80) {
        Sound_PlaySong(0xDC);
        p->selection++;
        if (p->selection > 7)
            p->selection = 0;
    }
    if (gPadState[1] & 1) {
        if (p->selection == 1)
            Script_Call(gOptionMenuParameters.callbacks.entries[1], ap);
        else
            Script_Call(gOptionMenuParameters.callbacks.entries[p->selection], 0);
        Task_Kill(p);
    } else {
        p->timer++;
        if (sub_08249350(p->timer, 600) == 0) {
            Script_Call(gOptionMenuParameters.callbacks.entries[6], 0);
            Task_Kill(p);
        }
    }
}

void OptionMenu_OnDestroy(struct OptionMenu *menu)
{
}

s32 OptionMenu_Initialize(struct OptionMenu *menu)
{
    return 0;
}

void OptionMenu_SetScriptsFromScript(void)
{
    if (Script_SeekToKeyword('g'))
        gOptionMenuParameters.callbacks.keyword.g = Script_GetValue();
    if (Script_SeekToKeyword('o'))
        gOptionMenuParameters.callbacks.keyword.o = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gOptionMenuParameters.callbacks.keyword.s = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gOptionMenuParameters.callbacks.keyword.c = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        gOptionMenuParameters.callbacks.keyword.r = Script_GetValue();
    if (Script_SeekToKeyword('l'))
        gOptionMenuParameters.callbacks.keyword.l = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gOptionMenuParameters.callbacks.keyword.d = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gOptionMenuParameters.callbacks.keyword.f = Script_GetValue();
}

struct OptionMenu *OptionMenu_Create(void)
{
    struct OptionMenu *p;
    p = Task_Create(11, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))OptionMenu_Update,
                          (void (*)(void *))OptionMenu_OnDestroy);
        if (OptionMenu_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
