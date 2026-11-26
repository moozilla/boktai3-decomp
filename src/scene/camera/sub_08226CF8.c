#include "link/session/link_session_state.h"
#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
#include "engine/engine.h"
#include "player/player.h"
#include "ui/hud/scene_indicator.h"

struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Div(s32, s32);

// Moves a secondary camera point (pos) by mode: follow the player, hold, pan to `target`,
// or ease back to the player.
void sub_08226CF8(struct SceneCameraController *camera)
{
    struct GameState *player = gGameState;
    struct PackedVec target;
    target.x = player->positionX + camera->offsetX;
    target.y = player->positionY + camera->offsetY;
    target.z = player->positionZ + camera->offsetZ;
    switch (camera->secondaryMode) {
        case 0:
            if (gEngineFlags & 0x800) {
                struct AreaEffectLinkState *link = gLinkSession;
                {
                    s16 index;
                    if (link && (index = link->localPlayer) >= 0) {
                        struct Player **table = gPlayers;
                        struct Player **p = table + index;
                        if (*p) {
                            camera->saved.x = (*p)->position.x;
                            camera->saved.y = (*p)->position.y + 150;
                            camera->saved.z = (*p)->position.z;
                        }
                    }
                }
            } else {
                if (gCamera.mode == 0) {
                    camera->saved.x = target.x;
                    camera->saved.y = target.y + 150;
                } else {
                    camera->saved.x = target.x;
                    camera->saved.y = target.y - 150;
                }
                camera->saved.z = target.z;
            }
            break;

        case 1:
            camera->saved.x = camera->target.x;
            camera->saved.y = camera->target.y;
            camera->saved.z = camera->target.z;
            break;

        case 2:
            camera->saved.x =
                Div((camera->saved.x * (camera->secondaryTimer - 1)) + camera->target.x, camera->secondaryTimer);
            camera->saved.y =
                Div((camera->saved.y * (camera->secondaryTimer - 1)) + camera->target.y, camera->secondaryTimer);
            camera->saved.z =
                Div((camera->saved.z * (camera->secondaryTimer - 1)) + camera->target.z, camera->secondaryTimer);
            if ((--camera->secondaryTimer) <= 1)
                camera->secondaryMode = 1;
            break;

        case 3:
            camera->saved.x = Div((camera->saved.x * (camera->secondaryTimer - 1)) + target.x, camera->secondaryTimer);
            camera->saved.y =
                Div(((camera->saved.y * (camera->secondaryTimer - 1)) + target.y) + 150, camera->secondaryTimer);
            camera->saved.z = Div((camera->saved.z * (camera->secondaryTimer - 1)) + target.z, camera->secondaryTimer);
            if ((--camera->secondaryTimer) <= 1)
                camera->secondaryMode = 0;
            break;
    }
}
