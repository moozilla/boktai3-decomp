#ifndef GUARD_INCLUDE_LINK_RECORDS_RECORD_NAME_PANEL_H
#define GUARD_INCLUDE_LINK_RECORDS_RECORD_NAME_PANEL_H

#include "global.h"

struct RecordNamePanel {
    u8 pad0[0xcec];
    const u8 *labelScript; /* confidence=low */
    u8 padCF0[0x4];
    const u8 *nameScript;
    u8 padCF8[0x10];
    s32 win[7]; /* confidence=low */
    u8 padD24[0x2];
    s8 page; /* confidence=low */
    u8 padD27[0x1];
};

typedef char RecordNamePanelSizeCheck[sizeof(struct RecordNamePanel) == 0xd28 ? 1 : -1];

#endif
