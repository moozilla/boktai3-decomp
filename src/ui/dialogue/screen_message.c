#include "ui/dialogue/screen_message.h"
#include "engine/video/palette_pulse.h"

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
                if (gScreenParams.showChoices != 0) {
                    s->choices[0].sprite.flags |= 1;
                    s->choices[1].sprite.flags |= 1;
                }
                if (s->result == 0)
                    Script_Call(gScreenParams.cancelScript, 0);
                else if (s->result == 1)
                    Script_Call(gScreenParams.confirmScript, 0);
                else
                    Script_Call(gScreenParams.alternateScript, 0);
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
    u32 size;
    void *resource;

    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    resource = ScriptProc_Start(0xC091, 0xA597);
    size = 3;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &size);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct ScreenMessagePalette *)ScriptProc_Start(0x92B3, 0x81B))->colors,
           gBgPaletteBuffer, 0x100);
    if (gScreenParams.showChoices == 1) {
        void *choiceResource = ScriptProc_Start(0xCB05, 0x530D);

        ResourceHeader_Relocate(&s->choices[0].graphics, choiceResource);
        AnimSprite_InitLinked(&s->choices[0].sprite, &s->choices[0].graphics, 0x4B, 0x10, 0, 0, 0,
                              0);
        ResourceHeader_Relocate(&s->choices[1].graphics, choiceResource);
        AnimSprite_InitLinked(&s->choices[1].sprite, &s->choices[1].graphics, 0x4C, 0x10, 0, 0, 0,
                              0);
        s->choices[0].sprite.position.x = -8;
        s->choices[0].sprite.position.y = -64;
        s->choices[1].sprite.position.x = -8;
        s->choices[1].sprite.position.y = -64;
        s->choices[0].sprite.flags |= 1;
        s->choices[1].sprite.flags |= 1;
        s->originalPalette = s->choices[0].sprite.palette;
        CpuSet(s->originalPalette, s->palette, 0x10);
        s->choices[0].sprite.palette = s->palette;
        s->choices[1].sprite.palette = s->palette;
    }
    s->textWindow = TextWindow_Create(1, 6, 28, 6);
    TextWindow_SetScript(s->textWindow, gScreenParams.script);
    TextWindow_Refresh(s->textWindow, gScreenParams.message);
    TextWindow_StartPlayback(s->textWindow);
    s->fadeSpeed = 0;
    Menu_StartFadeIn(0);
    s->selection = 1;
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
