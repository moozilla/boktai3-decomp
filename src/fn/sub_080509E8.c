#include "render.h"
#include "orbit_background.h"
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline s32 absolute(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}
void OrbitBackground_UpdatePosition(struct OrbitBackground *background)
{
    s32 finalZ;
    if (background->holdCamera) {
        if (background->blendFrame <= 6) {
            s32 weight, denominator;
            background->blendFrame++;
            weight = 8 - background->blendFrame;
            denominator = weight + 1;
            background->position.x =
                Div(background->position.x * weight + gCamera.position.x, denominator);
            background->position.y =
                Div(background->position.y * weight + gCamera.position.y, denominator);
            finalZ = Div(background->position.z * weight + gCamera.position.z, denominator);
        } else {
            background->blendFrame = 8;
            background->position.x = gCamera.position.x;
            background->position.y = gCamera.position.y;
            finalZ = (u16)gCamera.position.z;
        }
    } else {
        if (gPlayers[background->playerIndex]->cameraOffsetActive) {
            if (gPlayers[background->playerIndex]->tetherMode == 4)
                background->angle = (gGameState->unk85E + 128) & 255;
            else
                background->angle = gGameState->unk85E;
            if (background->blendFrame <= 6) {
                s32 weight, denominator;
                background->blendFrame++;
                weight = 8 - background->blendFrame;
                denominator = weight + 1;
                background->position.x = Div(background->position.x * weight +
                                                 gPlayers[background->playerIndex]->cameraTarget.x,
                                             denominator);
                background->position.y = Div(background->position.y * weight +
                                                 gPlayers[background->playerIndex]->cameraTarget.y,
                                             denominator);
                finalZ = Div(background->position.z * weight +
                                 gPlayers[background->playerIndex]->cameraTarget.z,
                             denominator);
            } else {
                background->blendFrame = 8;
                background->position.x = gPlayers[background->playerIndex]->cameraTarget.x;
                background->position.y = gPlayers[background->playerIndex]->cameraTarget.y;
                finalZ = (u16)gPlayers[background->playerIndex]->cameraTarget.z;
            }
        } else {
            s32 source = gGameState->unk85E;
            s32 angle = background->angle;
            s32 difference;
            s32 reverse;
            s32 step;
            {
                s32 adjusted = angle - 256;
                difference = (source - adjusted) & 255;
            }
            reverse = 128 - difference;
            if (absolute(reverse) <= 16) {
                if (difference != 128) {
                    if (difference <= 127) {
                        step = (reverse >> 2) + 1;
                        background->angle = (angle + step) & 255;
                    } else {
                        s32 next = difference - 128;
                        next >>= 2;
                        next = angle - next;
                        next += 255;
                        background->angle = next & 255;
                    }
                }
                {
                    s32 radius = (background->radius * 3 - 300) >> 2;
                    background->radius = radius;
                    if ((s16)radius < 0) {
                        background->radius = -radius;
                        background->angle = gGameState->unk85E;
                    }
                }
            } else {
                if (difference) {
                    if (difference <= 127) {
                        step = (difference >> 2) + 1;
                        background->angle = (angle + step) & 255;
                    } else {
                        step = ((256 - difference) >> 2) + 1;
                        background->angle = (angle + 256 - step) & 255;
                    }
                }
                if (background->radius <= 299)
                    background->radius = (background->radius * 3 + 300) >> 2;
            }
            {
                if (background->blendFrame > 1) {
                    s32 denominator;
                    background->target.x =
                        (u16)gPlayers[background->playerIndex]->position.x +
                        scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
                    {
                        struct Player *player = gPlayers[background->playerIndex];
                        background->target.y = (((128 - player->visualSlowdownTimer) * 150) >> 7) +
                                               (u16)player->position.y;
                    }
                    background->target.z =
                        (u16)gPlayers[background->playerIndex]->position.z +
                        scale(gUnk_086149C4[background->angle & 255] * background->radius);
                    background->blendFrame--;
                    denominator = background->blendFrame + 1;
                    background->position.x =
                        Div(background->position.x * background->blendFrame + background->target.x,
                            denominator);
                    background->position.y =
                        Div(background->position.y * background->blendFrame + background->target.y,
                            denominator);
                    finalZ =
                        Div(background->position.z * background->blendFrame + background->target.z,
                            denominator);
                } else {
                    background->blendFrame = 0;
                    background->position.x =
                        (u16)gPlayers[background->playerIndex]->position.x +
                        scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
                    {
                        struct Player *player = gPlayers[background->playerIndex];
                        background->position.y =
                            (((128 - player->visualSlowdownTimer) * 150) >> 7) +
                            (u16)player->position.y;
                    }
                    finalZ = (u16)gPlayers[background->playerIndex]->position.z +
                             scale(gUnk_086149C4[background->angle & 255] * background->radius);
                }
            }
        }
    }
    background->position.z = finalZ;
}
