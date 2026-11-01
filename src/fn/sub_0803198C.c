// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "player_cue_sequence.h"

struct MsgTextState {
    u8 pad0[9];
    u8 lineWidth;
    u8 pad0a[3];
    u8 valueCount;
    u8 pad0e[3];
    u8 spacing;
    u8 pad12[0xA];
    u32 lookup;
    u8 pad20[0xA8];
    u32 values[1];
};

struct MsgHdr {
    u32 w[8];
};

struct MsgWin {
    u8 pad00[0x18];
    struct MsgTextState state;
    u8 pad_e4[0x17C - 0xE4];
    u8 rectX, rectY, rectW, rectH;
    u16 scriptIndex;
    u8 mode;
    u8 flag183;
    u8 previousMode;
    u8 pad185;
    u8 scrollMode;
    u8 defaultMode;
    u16 scrollStep;
    u16 scrollCounter;
    u32 unk18C;
    u32 unk190;
    u16 palette;
    u16 wait;
    u8 pad198[0xC];
    void *script;
    u8 pad1A8[0x1F0 - 0x1A8];
    u8 hdr[0x20];
    u8 widget[0x60];
    u8 animData[0x1C];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    void *proc2C8;
    void (*fnDone)(void);
};

void MessageWindow_ClearTextCompletion(void);
void TextWindow_InitializeGraphics(void);
void TextWindow_FillTileGraphics(void);
void TextState_Init(void *, s32, s32, s32, s32);
s32 Script_GetValue(void);
void *Script_GetTextSourcePc(void);
s32 MessageWindow_LoadPalette(void *, s32);
void MessageWindow_ApplyPalette(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);

// confidence=low: opens a message window from script keywords (bounds, palette, text source, value list) and starts its script, sprite and animation setup.
int sub_0803198C(void *arg0, void *arg1)
{
    struct MsgWin *p = arg0;
    struct MsgTextState *st;
    u32 buf[2];
    void *proc;
    s32 palette;
    u32 bufferMask;
    void (**fnSlot)(void);

    gMessageWindow = p;
    st = &p->state;
    TextWindow_InitializeGraphics();
    TextWindow_FillTileGraphics();
    if (Script_SeekToKeyword('r')) {
        p->rectX = Script_GetValue();
        p->rectY = Script_GetValue();
        p->rectW = Script_GetValue();
        p->rectH = Script_GetValue();
    } else {
        p->rectX = 0;
        p->rectY = 0;
        p->rectW = 0;
        p->rectH = 0;
    }
    p->unk190 = 0;
    p->unk18C = Script_GetKeywordValue('l', 0);
    TextState_Init(st, p->rectX, p->rectY, p->rectW, p->rectH);
    p->mode = 0;
    if (Script_SeekToKeyword('s')) {
        void *pc = Script_GetTextSourcePc();
        p->script = pc;
        if (!pc)
            return -1;
        st->lookup = (u32)Text_LookupString(Script_ParseStringRef(pc));
        if (!st->lookup)
            return -1;
    } else {
        p->script = 0;
        st->lookup = 0;
    }
    palette = Script_GetKeywordValue('c', 0x519C);
    if (MessageWindow_LoadPalette(p, palette) < 0)
        return -1;
    MessageWindow_ApplyPalette(p);
    st->spacing = 0;
    p->pad185 = 0;
    p->scriptIndex = Script_GetKeywordValue('i', 0);
    p->defaultMode = 3;
    p->flag183 = 0;
    p->scrollMode = 0;
    p->scrollStep = 0;
    st->valueCount = 0;
    if (Script_SeekToKeyword('p')) {
        fnSlot = &p->fnDone;
        while (Script_GetPc()) {
            st->values[st->valueCount] = Script_GetValue();
            st->valueCount++;
        }
    } else {
        st->valueCount = 0;
        fnSlot = &p->fnDone;
    }
    p->wait = 0xFFFF;
    {
        register void (*callback)(void) asm("r1") = MessageWindow_ClearTextCompletion;
        *fnSlot = callback;
    }
    (void)*(volatile u8 *)&p->mode;
    proc = ScriptProc_Start(0xCB05, 0x530D);
    if (!proc)
        return -1;
    *(struct MsgHdr *)p->hdr = *(struct MsgHdr *)proc;
    ResourceHeader_Relocate(p->hdr, proc);
    bufferMask = 0xFFFF0000;
    buf[0] = 0;
    buf[1] &= bufferMask;
    Widget_InitLinked(p->widget, p->hdr, 0x3D, 0x3071, 0, 1, 0x3C, buf);
    Anim_SetKeyframes(p->widget, p->hdr, 0, 1, 0);
    AnimData_Load(p->animData, 0x593A);
    p->proc2C8 = ScriptProc_Start(0x922E, 0x4BBF);
    Sprite_Init(&p->sprite, p->animData, 0x1051);
    p->sprite.priority = 0;
    Anim_SetSequence(&p->anim, p->proc2C8, 0, 0, 0);
    PlayerCue_StepSequence(&p->sprite, &p->anim);
    p->sprite.position.x = 4;
    p->sprite.position.y = 0x40;
    p->sprite.position.z = 0;
    return 0;
}
