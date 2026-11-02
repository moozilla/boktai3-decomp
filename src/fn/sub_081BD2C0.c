#include "dark_loan_screen.h"
#include "engine.h"

s32 DarkLoan_OnUpdate(struct DarkLoanScreen *p)
{
    Callback_InvokeActor(p, p->callback);
    return 0;
}
