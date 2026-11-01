ContactActorPool_Start has a guarded, pin-free draft at 97.1%. The older
99.9% draft pinned the sample table to r8 and used a barrier on the no-target
sentinel. Both are gone. The setup stores the sentinel through the existing
signed target field and reuses its setup pointer for the random samples.
The draft is four bytes longer. The remaining differences concern when agbcc
hoists the random-table base relative to the cursor and health collider.
