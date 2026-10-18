#include "global.h"
void Script_ResetArgumentStack(void);
void Script_ResetKeywordStack(void);
void Script_ResetStacks(void)
{
    Script_ResetArgumentStack();
    Script_ResetKeywordStack();
}
