#include "engine/shared/global.h"
#include "engine/shared/link_session_state.h"

void Registry1745_ToggleConnectionRole(struct AreaEffectLinkState *session)
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
