#include "message_menu.h"

void MessageMenu_LoadBackground(struct MessageMenu *m, int id)
{
    void *map = ScriptProc_Start(0xC091, 0x3536);
    m->map = map;
    Bg_SetupLayers(1, 0, map, 0, 0, 1, &id);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    m->palette = (u8 *)ScriptProc_Start(0x92B3, 0x204) + 20;
    CpuSet(m->palette, gBgPaletteBuffer, 256);
}

int MessageMenu_CreateTextWindows(struct MessageMenu *m)
{
    int *handle = &m->mainText;
    int i;
    u8 *palette;
    *handle = TextWindow_Create(2, 5, 28, 4);
    if (*handle < 0)
        return -1;
    for (i = 0; i < 4; i++)
        TextWindow_SetString(*handle, i, m->lines[i]);
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 0);
    TextWindow_StartPlayback(*handle);
    handle = &m->footerText;
    *handle = TextWindow_Create(6, 17, 28, 2);
    if (*handle < 0)
        return -1;
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 1);
    TextWindow_StartPlayback(*handle);
    handle = &m->choiceText;
    *handle = TextWindow_Create(0, 11, 30, 4);
    if (*handle < 0)
        return -1;
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 2);
    TextWindow_StartPlayback(*handle);
    palette = ScriptProc_Start(0x92B3, 0x519C);
    if (!palette)
        return -1;
    CpuSet(palette + 500, gBgPaletteBank15, 0x04000008);
    return 0;
}

struct Position {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

void MessageMenu_InitializeTransition(struct MessageMenu *m)
{
    struct Position zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    Widget_InitLinked(m->transition, &m->resource, 57, 16, 0, 0, 60, &zero);
}

int MessageMenu_Update(struct MessageMenu *m)
{
    if (m->timer <= 40 && !(m->timer & 1))
        Sound_PlaySong(628);
    if (m->timer > 59 && (gPadState[1] & 8)) {
        Sound_PlaySong(221);
        if (m->script) {
            Script_Call(m->script, 0);
            m->script = 0;
        }
    }
    m->timer++;
    return 0;
}

int MessageMenu_Destroy(struct MessageMenu *m)
{
    int *handles = &m->mainText;
    int i = 2;
    do {
        TextWindow_Destroy(*handles++);
    } while (--i >= 0);
    Anim_Remove(m->transition);
    return 0;
}

int MessageMenu_Initialize(struct MessageMenu *m)
{
    struct MenuResource *r = ScriptProc_Start(0xCB05, 0xE2AB);
    if (!r)
        return -1;
    m->resource = *r;
    ResourceHeader_Relocate(&m->resource, r);
    m->timer = 0;
    if (!Script_SeekToKeyword('c'))
        return -1;
    m->source = Text_LookupString(Script_ParseStringRef(Script_GetPc()));
    if (!Script_SeekToKeyword('d'))
        return -1;
    m->text = Script_GetTextSourcePc();
    if (!m->text)
        return -1;
    m->script = Script_GetKeywordValue('e', 0);
    EncodedRecord_EncodeFields(m->formatter);
    EncodedRecord_FormatRows(m->lines, m->formatter, m->source, 6);
    MessageMenu_LoadBackground(m, 12);
    if (MessageMenu_CreateTextWindows(m) != 0)
        return -1;
    MessageMenu_InitializeTransition(m);
    return 0;
}

void MessageMenu_UpdateTask(void);

void MessageMenu_DestroyTask(void);

void *Password_Create(void)
{
    void *obj;
    obj = Task_Create(11, 0x4D8);
    if (obj != 0) {
        Task_SetCallbacks(obj, MessageMenu_Update, MessageMenu_Destroy);
        if (MessageMenu_Initialize(obj) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
