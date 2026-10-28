#include "global.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    s16 x, y, z, w;
};

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
void PlayerCue_BeginAcknowledgement(u8 *);
void PlayerCue_AcknowledgePendingTrigger(u8 *);
void PlayerCue_Hide(u8 *);
void PlayerCue_SetPosition(u8 *, struct Vec *);
void PlayerCue_Stand(u8 *, s32);
void PlayerCue_Face(u8 *, s32, s32);
void PlayerCue_Idle(u8 *, s32);
void PlayerCue_StandVariant2(u8 *, s32, s32);
void PlayerCue_StandVariant3(u8 *, s32, s32);
void PlayerCue_WalkTo(u8 *, struct Vec *, s32, s32);
void PlayerCue_Motion7Front(u8 *, s32, s32);
void PlayerCue_Motion7(u8 *, s32, s32);
void PlayerCue_Motion7End(u8 *, s32);
void PlayerCue_Kneel(u8 *, s32, s32);
void PlayerCue_KneelDown(u8 *, s32);
void PlayerCue_KneelGasp(u8 *, s32);
void PlayerCue_GetUp(u8 *, s32, s32);
void PlayerCue_MoveTo(u8 *, struct Vec *, s32, s32, s32);
void PlayerCue_Motion29(u8 *, s32, s32, s32);
void PlayerCue_Motion30(u8 *, s32, s32, s32, s32);
void PlayerCue_Motion30End(u8 *, s32);
void PlayerCue_Motion29B(u8 *, s32, s32, s32);
void PlayerCue_Motion7Turn(u8 *, s32, s32);
void PlayerCue_Motion7Variant3(u8 *, s32, s32);
void PlayerCue_Motion7Variant7(u8 *, s32, s32);
void PlayerCue_Aim(u8 *, s32, s32);
void PlayerCue_Shoot(u8 *, s32, s32);
void PlayerCue_AimHold(u8 *, s32, s32, s32);
void PlayerCue_AimEnd(u8 *, s32);
void PlayerCue_Motion11(u8 *, s32);
s32 PlayerCue_Motion10(u8 *, s32);
s32 PlayerCue_FormMotion(u8 *, s32);
s32 PlayerCue_Motion38To(u8 *, struct Vec *, s32);
s32 PlayerCue_StateMotion(u8 *, s32);
void PlayerCue_RunTo(u8 *, struct Vec *, s32, s32);
void PlayerCue_Run(u8 *, s32);
void PlayerCue_Windblown(u8 *, s32, s32);
void PlayerCue_WindblownVariant3(u8 *, s32, s32);
void PlayerCue_WindblownVariant4(u8 *, s32, s32);
void PlayerCue_WindblownVariant2(u8 *, s32, s32);
void PlayerCue_StandDiagonal(u8 *, s32, s32);
void PlayerCue_StandDiagonalB(u8 *, s32, s32);
void PlayerCue_FaceSide(u8 *, s32);
void PlayerCue_Motion17(u8 *, s32);
void PlayerCue_Motion17Variant2(u8 *, s32);
void PlayerCue_ShakeEffect(u8 *, s32);
void PlayerCue_Refresh(u8 *);
void PlayerCue_OverlaySet(u8 *, s32);
void PlayerCue_OverlayOn(u8 *, s32);
void PlayerCue_OverlayOff(u8 *);
void sub_0815C73C(u8 *, s32, s32, s32);

void PlayerCue_Dispatch(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x2F8);
    s32 i;
    struct Vec v1, v7, v51, v33;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]])
        do {
            s16 *m;
            u8 *controller = gSceneEventDispatchState;
            m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    PlayerCue_Hide(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 1:
                    v1.x = m[4];
                    v1.y = m[5];
                    v1.z = m[6];
                    PlayerCue_SetPosition(p, &v1);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 2:
                    PlayerCue_Stand(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 3:
                    PlayerCue_Face(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 4:
                    PlayerCue_Idle(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 5:
                    PlayerCue_StandVariant2(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 6:
                    PlayerCue_StandVariant3(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 7:
                    v7.x = m[4];
                    v7.y = m[5];
                    v7.z = m[6];
                    PlayerCue_WalkTo(p, &v7, 0, -1);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 8:
                    PlayerCue_Motion7Front(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 9:
                    PlayerCue_Motion7(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 10:
                    PlayerCue_Motion7End(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 11:
                    PlayerCue_Kneel(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 12:
                    PlayerCue_KneelDown(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 13:
                    PlayerCue_KneelGasp(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 14:
                    PlayerCue_GetUp(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 15:
                    PlayerCue_MoveTo(p, 0, m[4], 0, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 16:
                    PlayerCue_Motion29(p, m[4], m[5], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 17:
                    PlayerCue_Motion30(p, m[4], m[5], m[6], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 18:
                    PlayerCue_Motion30End(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 19:
                    PlayerCue_Motion29B(p, m[4], m[5], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 20:
                    PlayerCue_Motion7Turn(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 22:
                    PlayerCue_Motion7Variant3(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 21:
                    PlayerCue_Motion7Variant7(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 23:
                    PlayerCue_Aim(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 24:
                    PlayerCue_Shoot(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 25:
                    PlayerCue_AimHold(p, m[4], m[5], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 26:
                    PlayerCue_AimEnd(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 28:
                    PlayerCue_Motion11(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 29: {
                    s32 r = PlayerCue_Motion10(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 30: {
                    s32 r = PlayerCue_FormMotion(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 51:
                    v51.x = m[4];
                    v51.y = m[5];
                    v51.z = m[6];
                    PlayerCue_Motion38To(p, &v51, 0);
                    break;
                case 31: {
                    s32 r = PlayerCue_StateMotion(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    if (r == 0)
                        PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                }
                case 33:
                    v33.x = m[4];
                    v33.y = m[5];
                    v33.z = m[6];
                    PlayerCue_RunTo(p, &v33, -1, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 34:
                    PlayerCue_Run(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 35:
                    PlayerCue_Windblown(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 37:
                    PlayerCue_WindblownVariant3(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 38:
                    PlayerCue_WindblownVariant4(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 36:
                    PlayerCue_WindblownVariant2(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 39:
                    PlayerCue_StandDiagonal(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 40:
                    PlayerCue_StandDiagonalB(p, m[4], 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 27:
                    PlayerCue_FaceSide(p, m[4]);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 41:
                    PlayerCue_Motion17(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 42:
                    PlayerCue_Motion17Variant2(p, 0);
                    PlayerCue_BeginAcknowledgement(p);
                    break;
                case 43:
                    PlayerCue_ShakeEffect(p, 2);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 44:
                    PlayerCue_Refresh(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 48:
                    PlayerCue_OverlaySet(p, m[4] + 0x16E);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 47:
                    PlayerCue_OverlayOn(p, m[4] + 0x16E);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 49:
                    PlayerCue_OverlayOff(p);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 50:
                    PlayerCue_OverlaySet(p, 0x17A);
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 45:
                    p[0x5A1] = 1;
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 46:
                    p[0x5A1] = 0;
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
                case 32:
                    switch (m[4]) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                            sub_0815C73C(p, m[4], 0, 0);
                            break;
                        case 4:
                        case 5:
                            sub_0815C73C(p, m[4], m[5], m[6]);
                            break;
                    }
                    PlayerCue_BeginAcknowledgement(p);
                    PlayerCue_AcknowledgePendingTrigger(p);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
}
