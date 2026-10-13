#include "global.h"
struct Sprite {
    u32 flags;
    u8 pad4[3], visible;
    u8 pad8[8];
    u16 tile;
    u8 pad12[10];
    u16 x, y;
    u8 pad20[12];
};
struct Animation {
    const u16 *frames;
    u8 flags, length, duration, period;
    u16 frame, base, speed, timer;
};
struct PeerPanel {
    u8 active, pad1[3];
    u32 flags;
    struct Sprite sprite;
    u8 glyph[0x1C];
    u8 drawings[5][0x60];
    u8 name[16];
    u8 pad240[4];
    u8 changed;
};
struct PeerStyle {
    int resource;
    u8 tile, animation, priority, x, y, pad9[3];
};
extern const struct PeerStyle gUnk_0824DE64[];
void *ScriptProc_Start(int, int);
void AnimData_Load(void *, int);
void AnimData_SelectTiles(void *, int);
void Sprite_Init(void *, void *, int);
void sub_08220D20(void *, void *, int, int, int);
void sub_0821983C(void *, void *, int, int, int, int, int, int);
void Sprite_Remove(void *);
void Anim_Remove(void *);
void Memory_Copy(void *, const void *, int);
void sub_08036CF0(const u8 *, u8 *, int);
void *sub_08037DCC(int);
void TextWindow_Refresh(int, int);
void sub_0803746C(void *owner, struct PeerPanel *p)
{
    u8 *drawing;
    int i;
    Sprite_Remove(&p->sprite);
    drawing = p->drawings[0];
    i = 4;
    do {
        Anim_Remove(drawing);
        drawing += 0x60;
    } while (--i >= 0);
}
