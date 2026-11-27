#ifndef GUARD_INCLUDE_LINK_LINK_SCORE_PANEL_H
#define GUARD_INCLUDE_LINK_LINK_SCORE_PANEL_H

#include "global.h"

struct LinkScorePanel {
    u8 f[0x1026]; /* confidence=low */
    u16 h; /* confidence=low */
    u8 p[6]; /* confidence=low */
    u16 h2;
};

typedef char LinkScorePanelSizeCheck[sizeof(struct LinkScorePanel) == 0x1030 ? 1 : -1];

#endif
