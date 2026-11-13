#include "screen_message.h"

void ScreenMessage_UpdateChoiceHighlight(struct ScreenMessage *p)
{
    if (p->selection == 0) {
        p->choices[0].sprite.flags &= ~1;
        p->choices[1].sprite.flags |= 1;
    } else {
        p->choices[0].sprite.flags |= 1;
        p->choices[1].sprite.flags &= ~1;
    }
}

void ScreenMessage_Update(struct ScreenMessage *s)
{
    struct ScreenParams *g;

    switch (s->state) {
        case 0:
            if (gPadState[1] & 1) {
                if (s->selection == 0 || gScreenParams.showChoices == 0) {
                    Sound_PlaySong(0xDD);
                    TextWindow_Destroy(s->textWindow);
                    if (gScreenParams.showChoices != 0) {
                        s->choices[0].sprite.flags |= 1;
                        s->choices[1].sprite.flags |= 1;
                    }
                    Script_Call(gScreenParams.confirmScript, 0);
                    Task_Kill(s);
                } else {
                    Sound_PlaySong(0xDE);
                    TextWindow_Destroy(s->textWindow);
                    if (gScreenParams.showChoices != 0) {
                        s->choices[0].sprite.flags |= 1;
                        s->choices[1].sprite.flags |= 1;
                    }
                    Script_Call(gScreenParams.alternateScript, 0);
                    Task_Kill(s);
                }
            } else if (gPadState[1] & 2) {
                if (gScreenParams.showChoices == 0) {
                    Sound_PlaySong(0xDD);
                    TextWindow_Destroy(s->textWindow);
                    Script_Call(gScreenParams.confirmScript, 0);
                    Task_Kill(s);
                } else {
                    Sound_PlaySong(0xDE);
                    TextWindow_Destroy(s->textWindow);
                    s->choices[0].sprite.flags |= 1;
                    s->choices[1].sprite.flags |= 1;
                    Script_Call(gScreenParams.cancelScript, 0);
                    Task_Kill(s);
                }
            }
            if (gScreenParams.showChoices == 1) {
                if (((gPadState[1] & 0x20) && s->selection == 1) ||
                    ((gPadState[1] & 0x10) && s->selection == 0)) {
                    s->selection = 1 - s->selection;
                    Sound_PlaySong(0x37C);
                }
                ScreenMessage_UpdateChoiceHighlight(s);
                Palette_UpdateHighlightPulse(s->palette + 12, &s->pulseState);
            }
            Palette_SetBackdropColor(0x73FF);
            break;
        case 1:
            if (s->fadeTimer > (1 << s->fadeSpeed) || s->fadeSpeed == 0) {
                TextWindow_Destroy(s->textWindow);
                g = &gScreenParams;
                if (g->showChoices != 0) {
                    s->choices[0].sprite.flags |= 1;
                    s->choices[1].sprite.flags |= 1;
                }
                if (s->result == 0)
                    Script_Call(g->cancelScript, 0);
                else if (s->result == 1)
                    Script_Call(g->confirmScript, 0);
                else
                    Script_Call(g->alternateScript, 0);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 2:
            break;
    }
}

void ScreenMessage_Destroy(struct ScreenMessage *p)
{
    if (gScreenParams.showChoices == 1) {
        Anim_Remove(&p->choices[1].sprite);
        Anim_Remove(&p->choices[0].sprite);
    }
}

s32 ScreenMessage_Initialize(struct ScreenMessage *s)
{
    u32 zero, one, value;
    void *a, *p, *resource;
    struct SceneInputResource *obj;
    struct SceneInputResource *obj2;
    struct ScreenParams *global;
    u32 *save;
    u8 *dest;
    u32 old;
    u32 *sound;
    u32 result;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    a = ScriptProc_Start(0xC091, 0xA597);
    value = 3;
    Bg_SetupLayers(2, 0, a, 0, zero, one = 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = ((struct ScreenMessagePalette *)ScriptProc_Start(0x92B3, 0x81B))->colors;
    CpuSet(p, gBgPaletteBuffer, 0x100);
    global = &gScreenParams;
    if (global->showChoices == 1) {
        u32 x, y;
        resource = ScriptProc_Start(0xCB05, 0x530D);
        obj = &s->choices[0].graphics;
        ResourceHeader_Relocate(obj, resource);
        Widget_InitLinked(&s->choices[0].sprite, obj, 0x4B, 0x10, zero, zero, zero, zero);
        obj2 = &s->choices[1].graphics;
        ResourceHeader_Relocate(obj2, resource);
        Widget_InitLinked(&s->choices[1].sprite, obj2, 0x4C, 0x10, zero, zero, zero, zero);
        x = 0xFFF8;
        *(u16 *)&s->choices[0].sprite.position.x = x;
        *(u16 *)&s->choices[0].sprite.position.y = y = 0xFFC0;
        *(u16 *)&s->choices[1].sprite.position.x = x;
        *(u16 *)&s->choices[1].sprite.position.y = y;
        s->choices[0].sprite.flags |= one;
        s->choices[1].sprite.flags |= one;
        save = (u32 *)&s->originalPalette;
        old = (u32)s->choices[0].sprite.palette;
        *save = old;
        dest = (u8 *)s->palette;
        CpuSet((void *)old, dest, 0x10);
        s->choices[0].sprite.palette = dest;
        s->choices[1].sprite.palette = dest;
    }
    result = TextWindow_Create(1, 6, 28, 6);
    sound = &s->textWindow;
    *sound = result;
    TextWindow_SetScript(result, global->script);
    TextWindow_Refresh(*sound, global->message);
    TextWindow_StartPlayback(*sound);
    s->fadeSpeed = zero;
    Menu_StartFadeIn(0);
    s->selection = one;
    Palette_SetBackdropColor(0x73FF);
    return 0;
}

void ScreenMessage_SetParametersFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScreenParams.script = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        gScreenParams.message = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gScreenParams.showChoices = Script_GetValue();
    if (Script_SeekToKeyword('y'))
        gScreenParams.confirmScript = Script_GetValue();
    if (Script_SeekToKeyword('n'))
        gScreenParams.alternateScript = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScreenParams.cancelScript = Script_GetValue();
}

struct ScreenMessage *ScreenMessage_Create(void)
{
    struct ScreenMessage *r = Task_Create(0xb, sizeof(*r));
    if (r != 0) {
        Task_SetCallbacks(r, (void (*)(void *))ScreenMessage_Update,
                          (void (*)(void *))ScreenMessage_Destroy);
        if (ScreenMessage_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
