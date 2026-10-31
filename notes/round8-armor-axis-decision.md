# Armor axis attack decision

Armor_UpdateAxisAttackDecision matches all 956 bytes at 08187230 with stock
agbcc. In the facing-Z arm of decision zero, handle both outcomes where they
are selected. A nearby player starts behavior 3 immediately. A player within
three grid cells starts behavior 10. Both paths set the decision byte and
return. The compiler later merges their common instructions with other cases.

Sharing these two exits explicitly in C instead changes the lifetime of the
signed X delta across the facing check, previously leaving 19 different bytes.
