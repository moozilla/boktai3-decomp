#include "global.h"
#include "ram.h"

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

struct MsgAnim {
    u16 *frames;
    u8 flags;
    u8 count;
    u8 nibble;
    u8 scaled;
    u16 frameIdx;
    u16 unkA;
    u16 speed;
    u16 counter;
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
    u8 sprite[0x2C];
    struct MsgAnim anim;
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
void Anim_SetKeyframes(void *, void *, u32, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);

#ifdef NONMATCHING
// fuzzy: 73.3%
// scope shape 41 (differences once low registers are set aside)
// confidence=low: opens a message window from script keywords (bounds, palette, text source, value list) and starts its script, sprite and animation setup.
int sub_0803198C(void *arg0, void *arg1)
{
    struct MsgWin *p = arg0;
    struct MsgTextState *st;
    u32 buf[2];
    void *proc;
    struct MsgAnim *a;
    u16 *fr;
    s32 palette;
    u32 bit = 1;
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
    *fnSlot = MessageWindow_ClearTextCompletion;
    proc = ScriptProc_Start(0xCB05, 0x530D);
    if (!proc)
        return -1;
    *(struct MsgHdr *)p->hdr = *(struct MsgHdr *)proc;
    ResourceHeader_Relocate(p->hdr, proc);
    buf[0] = 0;
    buf[1] &= 0xFFFF0000;
    Widget_InitLinked(p->widget, p->hdr, 0x3D, 0x3071, 0, 1, 0x3C, buf);
    Anim_SetKeyframes(p->widget, p->hdr, 0, 1);
    AnimData_Load(p->animData, 0x593A);
    p->proc2C8 = ScriptProc_Start(0x922E, 0x4BBF);
    Sprite_Init(p->sprite, p->animData, 0x1051);
    p->sprite[7] = 0;
    Anim_SetSequence(&p->anim, p->proc2C8, 0, 0, 0);
    a = &p->anim;
    fr = a->frames + a->frameIdx;
    *(u16 *)(p->sprite + 0x10) = *fr >> 6;
    if ((a->flags & bit) != ((*fr & 0x30) >> 4 & bit))
        *(u32 *)p->sprite |= 4;
    else
        *(u32 *)p->sprite &= ~4;
    if ((a->flags & 2) != ((*fr & 0x30) >> 4 & 2))
        *(u32 *)p->sprite |= 8;
    else
        *(u32 *)p->sprite &= ~8;
    a->counter++;
    if (a->counter >= a->scaled) {
        a->counter = 0;
        if (a->flags & 4) {
            if (a->frameIdx == 0) {
                if (a->flags & 8) {
                    a->counter = a->scaled;
                    goto update;
                }
                a->frameIdx = a->count - 1;
            } else {
                a->frameIdx--;
            }
        } else {
            a->frameIdx++;
            if ((u16)a->frameIdx >= a->count) {
                if (a->flags & 8) {
                    a->counter = a->scaled;
                    a->frameIdx = a->count - 1;
                } else {
                    a->frameIdx = 0;
                }
            }
        }
    update:
        fr = a->frames + a->frameIdx;
        {
            u16 v = *fr;
            a->nibble = v & 0xF;
        }
        a->scaled = (a->nibble * a->speed) >> 6;
        if ((u8)a->scaled == 0)
            a->scaled = 1;
    }
    *(u16 *)(p->sprite + 0x1C) = 4;
    *(u16 *)(p->sprite + 0x1E) = 0x40;
    *(u16 *)(p->sprite + 0x20) = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803198C);
#endif
