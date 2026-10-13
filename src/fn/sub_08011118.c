#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Piece {
    u8 active, state, done, angle;
    u16 timer;
    s8 direction;
    u8 pad7, speed, pad9;
    u16 targetY, duration, pade;
    struct Vec position, target;
    u8 pad20[32];
    u32 spriteFlags;
    u8 spritePad[24];
    struct Vec spritePosition;
    u8 pad64[20];
    void *palette;
    u8 pad7c[12];
    u8 transform[64];
};
struct Manager {
    u8 task[24];
    u32 active, timer;
    void *animation;
    struct Piece pieces[16];
    s8 paletteActive, paletteMask;
    u16 actorId;
    u8 palette[32];
    void *actor;
    struct Vec origin;
};
extern u8 *gUnk_030042E4;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern s16 gUnk_086149C4[];
void sub_08221110(void *, void *, void *, int, int);
int Mod(int, int);
void sub_0800FA5C(void *, void *, int);
void AnimData_SelectTiles(void *, int);
void ActorShadow_Enable(void *);
static inline int randomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct Vec *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
void *ScriptProc_Start(int, int);
void BoardPiece_Initialize(void *, void *, int);
void sub_0800FA40(void *, void *, int);
void SceneMessages_RegisterReceiver(void *, int, int);
void sub_08020D08(void *);
void *sub_08225984(int);
void BoardPieces_DispatchCommands(void *);
void BoardPiece_UpdateAnimation(void *);
extern void (*gUnk_08604B48[])(void *, void *, int);
int GroupObjects_Initialize(struct Manager *m, int id)
{
    struct Piece *p;
    int i;
    m->active = 0;
    m->actorId = 0;
    m->actor = 0;
    m->animation = ScriptProc_Start(0x922E, 0x5BB7);
    {
        void *first = gUnk_030042E4 + 0x5A60;
        void *second = gUnk_030042E4 + 0x5AA0;
        sub_08221110(m->palette, first, second, 64, 6);
    }
    p = m->pieces;
    for (i = 0; i < 16; i++, p++)
        BoardPiece_Initialize(m, p, i);
    SceneMessages_RegisterReceiver((u8 *)m + 0xCD4, id, 10);
    return 0;
}
