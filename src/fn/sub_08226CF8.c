#include "global.h"
#include "game_state.h"
#include "engine.h"

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Camera2 {
    u8 unknown0[0x54];
    u16 mode, timer;
    struct Vec pos;
    struct Vec target;
    u8 unknown68[0xC];
    s16 offsetX, offsetY, offsetZ;
};
struct Player {
    u8 unknown0[0x30];
    s16 x, y, z;
};
struct LinkState {
    u8 unknown0[0x18];
    s16 player;
};
struct CameraState {
    struct Vec position;
    u8 unknown8[10];
    s16 facing;
};
extern struct LinkState *gUnk_020004B4;
extern struct Player *gUnk_02000580[4];
extern struct CameraState gUnk_030042A0;
s32 Div(s32, s32);

// Moves a secondary camera point (pos) by mode: follow the player, hold, pan to `target`,
// or ease back to the player.
void sub_08226CF8(struct Camera2 *camera)
{
    struct Player *player = gGameState;
    struct PackedVec target;
    target.x = player->x + camera->offsetX;
    target.y = player->y + camera->offsetY;
    target.z = player->z + camera->offsetZ;
    switch (camera->mode) {
        case 0:
            if (gEngineFlags & 0x800) {
                struct LinkState *link = gUnk_020004B4;
                {
                    s16 index;
                    if (link && (index = link->player) >= 0) {
                        struct Player **table = gUnk_02000580;
                        struct Player **p = table + index;
                        if (*p) {
                            camera->pos.x = (*p)->x;
                            camera->pos.y = (*p)->y + 150;
                            camera->pos.z = (*p)->z;
                        }
                    }
                }
            } else {
                if (gUnk_030042A0.facing == 0) {
                    camera->pos.x = target.x;
                    camera->pos.y = target.y + 150;
                } else {
                    camera->pos.x = target.x;
                    camera->pos.y = target.y - 150;
                }
                camera->pos.z = target.z;
            }
            break;

        case 1:
            camera->pos.x = camera->target.x;
            camera->pos.y = camera->target.y;
            camera->pos.z = camera->target.z;
            break;

        case 2:
            camera->pos.x =
                Div((camera->pos.x * (camera->timer - 1)) + camera->target.x, camera->timer);
            camera->pos.y =
                Div((camera->pos.y * (camera->timer - 1)) + camera->target.y, camera->timer);
            camera->pos.z =
                Div((camera->pos.z * (camera->timer - 1)) + camera->target.z, camera->timer);
            if ((--camera->timer) <= 1)
                camera->mode = 1;
            break;

        case 3:
            camera->pos.x = Div((camera->pos.x * (camera->timer - 1)) + target.x, camera->timer);
            camera->pos.y =
                Div(((camera->pos.y * (camera->timer - 1)) + target.y) + 150, camera->timer);
            camera->pos.z = Div((camera->pos.z * (camera->timer - 1)) + target.z, camera->timer);
            if ((--camera->timer) <= 1)
                camera->mode = 0;
            break;
    }
}
