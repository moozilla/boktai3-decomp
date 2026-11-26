#include "global.h"
#include "link/session/link_session_state.h"

void LinkSession_ToggleConnectionRole(struct AreaEffectLinkState *session)
{
    u8 page = session->selectionPage;
    if (page == 0) {
        session->selectionPage = 1;
        session->previousSelectionPage = page;
    } else {
        session->selectionPage = 0;
        session->previousSelectionPage = 1;
    }
}
