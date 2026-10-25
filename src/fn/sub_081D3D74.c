#include "global.h"

struct SceneDisplayConfig {
    u8 unk_00[0xE];
    u8 direction;
};
struct SceneDisplayPosition {
    u16 x, y;
    u32 unk_04;
};
struct SceneDisplay {
    u8 unk_00[0x20];
    u32 timer;
    u8 phase;
    u8 unk_25[3];
    u8 animation[0x8C];
    struct SceneDisplayPosition *position;
    u8 unk_B8[0x9EC - 0xB8];
    struct SceneDisplayPosition home;
    u8 unk_9F4[0xA];
    u8 dirX, dirY;
    u16 scale;
    u8 unk_A02[6];
    struct SceneDisplayConfig *config;
};
void Dir_Decompose(u8, u8 *, u8 *, u32);
s32 DisplayAnimation_SetEntry(void *, s32, s32, s32, s32, s32, s32);
s32 DisplayAnimation_UpdatePlayback(void *);

void SceneDisplay_UpdateHopAnimation(struct SceneDisplay *p)
{
    u32 timer = p->timer;
    s32 first = timer == 0;
    struct SceneDisplayConfig *config;
    s32 finished;
    p->timer = timer + 1;
    config = p->config;
    switch (p->phase) {
        case 0:
            if (first) {
                Dir_Decompose(config->direction, &p->dirX, &p->dirY, p->scale);
                DisplayAnimation_SetEntry(p->animation, 1, p->dirX, p->dirY, 2, 0, 4);
            }
            finished = DisplayAnimation_UpdatePlayback(p->animation);
            if (finished) {
                p->timer = 0;
                p->phase = 1;
            }
            break;
        case 1:
            if (first) {
                Dir_Decompose(config->direction, &p->dirX, &p->dirY, p->scale);
                DisplayAnimation_SetEntry(p->animation, 2, p->dirX, p->dirY, 2, 0, 4);
            }
            finished = DisplayAnimation_UpdatePlayback(p->animation);
            if (p->timer < 12)
                p->position->y--;
            else
                p->position->y++;
            if (finished) {
                p->timer = 0;
                p->phase = 2;
            }
            break;
        case 2:
            if (first) {
                Dir_Decompose(config->direction, &p->dirX, &p->dirY, p->scale);
                DisplayAnimation_SetEntry(p->animation, 3, p->dirX, p->dirY, 2, 0, 4);
                *p->position = p->home;
            }
            finished = DisplayAnimation_UpdatePlayback(p->animation);
            if (finished) {
                p->timer = 0;
                p->phase = 3;
            }
            break;
        case 3:
            if (p->timer > 30) {
                p->timer = 0;
                p->phase = 0;
            }
            break;
    }
}
