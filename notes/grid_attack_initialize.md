# Grid attack initialization

Tags: #matching #bosses 0802BFAC

GridAttackActor_Initialize matches all 1,652 bytes. The player loop fills
parallel actor and position slots. Caching the target and callback slot
addresses preserves the original 52-byte stack frame. A separate counter
for later loops lets agbcc keep the actor kind across script reads.

The initializer clears sprite flag bit zero. Its first palette loop reloads
the palette-bank pointer on each iteration. The other two loops repeat a
single bank across sixteen rows.
