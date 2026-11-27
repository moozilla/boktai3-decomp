#include "minigames/result_game/result_tutorial.h"

enum ResultMarkerFeedbackPhase {
    FEEDBACK_INACTIVE,
    FEEDBACK_ENTERING,
    FEEDBACK_HOLDING,
    FEEDBACK_LEAVING,
};

void ResultMenu_UpdateMarkers(struct ResultMenu *p)
{
    struct ResultMarkerFeedback *s = p->markerFeedback;
    struct AnimSprite *g = &p->widgets[6];
    int i;
    for (i = 0; i < 5; i++, s++, g++) {
        switch (s->phase) {
            case FEEDBACK_INACTIVE:
                break;
            case FEEDBACK_ENTERING:
                g->position.x = (8 - s->elapsed) * 8 + 4;
                if (s->elapsed > 7) {
                    g->position.x = 4;
                    s->phase = FEEDBACK_HOLDING;
                    s->elapsed = 0;
                }
                break;
            case FEEDBACK_HOLDING:
                if (s->elapsed > 63) {
                    s->phase = FEEDBACK_LEAVING;
                    s->elapsed = 0;
                    g->flags |= 0x200;
                }
                break;
            case FEEDBACK_LEAVING:
                if (s->elapsed > 7) {
                    s->phase = FEEDBACK_INACTIVE;
                    s->elapsed = 0;
                    g->flags = (g->flags & ~0x200) | 1;
                }
                break;
        }
        s->elapsed++;
    }
}
