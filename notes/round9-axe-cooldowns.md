# Axe cooldown thresholds

Tags: 080FAFD0 #axe #depin

`Axe_InitAttackCooldowns` matches all 128 bytes without pinning the
health load. The threshold subtracts one truncated signed quarter from the
original health. Expressing that calculation as a statement macro keeps
agbcc's regmove pass from forwarding the saved copy into the sign extension.
