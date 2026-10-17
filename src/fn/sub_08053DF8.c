// MATCHING_CONCESSION: register-pinning
#include "saved_position_marker.h"
void SavedPositionMarker_TickIdle(struct SavedPositionMarker *marker)
{
    u32 nearby;
    if (!(gUnk_030053F4 & 0x200) && marker->inputCooldown <= 9)
        marker->inputCooldown++;
    nearby = SavedPositionMarker_IsStoredPositionNearby(marker);
    if (nearby) {
        if (marker->nearby != 1) {
            Sound_PlaySong(0x198);
            marker->nearby = 1;
        }
        if (marker->activationFrame <= 7)
            marker->activationFrame++;
        if (marker->height <= 22)
            marker->height += 2;
        else
            marker->height = 24;
        {
            u32 button = 1;
            asm("" : "+r"(button));
            if (gUnk_02000560 & button) {
                u32 pressed = gUnk_03005260[1];
                register u32 edge asm("r0") = button;
                asm("" : "+r"(pressed), "+r"(edge));
                edge &= pressed;
                if (edge) {
                    struct InteractionMarkerPlayer *player = gUnk_02000580[0];
                    u32 blocked = 0x100000;
                    if (!(player->flags & blocked) && marker->inputCooldown > 9) {
                        marker->activationFrame = 8;
                        ScriptUtility_SetControlFlag(1);
                        PlayerCue_WalkTo(gUnk_02000580[0], &marker->position, 0, 4);
                        SavedPositionMarker_SetState(marker, SavedPositionMarker_TickInteraction);
                    }
                }
            }
        }
    } else {
        if (marker->nearby != 0) {
            Sound_PlaySong(0x199);
            marker->nearby = nearby;
        }
        if (marker->activationFrame != 0)
            marker->activationFrame--;
        if (marker->height > 2)
            marker->height -= 2;
        else
            marker->height = nearby;
    }
    if (marker->height == 0)
        marker->primary.sprite.flags |= 1;
    else {
        marker->primary.sprite.flags &= ~1;
        marker->primary.sprite.position.y = marker->position.y + marker->height;
    }
    if (marker->activationFrame > 7)
        AnimData_SelectPaletteBank(&marker->primary.graphics, 0x168);
    else
        AnimData_SelectPaletteBank(&marker->primary.graphics, marker->paletteBase);
}
