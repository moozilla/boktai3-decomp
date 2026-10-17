#include "global.h"
#include "engine.h"
#include "game_state.h"

struct Vec08196F58 {
    s16 x, y, z, w;
};
struct P08196F58 {
    u8 f0[0x18];
    u8 queue[0x5404 - 0x18];
    struct Vec08196F58 pos;
    struct Vec08196F58 base;
    s16 speed;
    u8 f1[0x724a - 0x5416];
    u8 state;
    u8 f2[0x7272 - 0x724b];
    u16 h7272;
    u8 f3[0x72a8 - 0x7274];
    s32 a, b, c, d, e, f;
    void *script;
};
void sub_08193680(void *, u32);
void sub_08195014(void *, s32, s32);
void sub_08195254(void *);
void Hud_SetMode(u32);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_08159164(u32);
void GameState_SetEventFlag(u32);
void Sound_PlaySongChecked(u32);

void sub_08196F58(struct P08196F58 *t)
{
    switch (t->state) {
        case 0:
            sub_08193680(t, 1);
            t->pos.x = -0xd80;
            t->pos.y = 0;
            t->pos.z = 0x80;
            t->pos.z = gUnk_02000710->positionZ + 0x80;
            t->base = t->pos;
            t->base.x = -0x680;
            sub_08195014(t, t->base.x, t->base.z);
            t->base.x += t->b >> 8;
            t->base.z += t->c >> 8;
            t->b = 0;
            t->c = 0;
            t->speed = 0;
            t->state++;
            Sound_PlaySongChecked(0x405);
            break;
        case 1: {
            s32 x, z;
            t->a += 0x3000;
            x = t->a + t->b;
            z = t->c;
            sub_08195254(t);
            t->pos.x = (x >> 8) + t->base.x;
            t->pos.z = (z >> 8) + t->base.z;
            if (t->a >= 0) {
                t->state++;
                Hud_SetMode(7);
                sub_082279A8(3, 4, 0x1f, 0x1f, 0x1f, 0xffff, 0);
            }
            break;
        }
        case 2: {
            s32 x, z;
            t->a += 0x3000;
            x = t->a + t->b;
            z = t->c;
            sub_08195254(t);
            t->pos.x = (x >> 8) + t->base.x;
            t->pos.z = (z >> 8) + t->base.z;
            if (t->a > 0x2ffff)
                t->state++;
            break;
        }
        case 3:
            sub_08159164(t->h7272);
            GameState_SetEventFlag(0x41);
            if (t->script)
                Script_Call(t->script, 0);
            Task_Kill(t);
            break;
    }
}
