# Formatter interruption regression

Tags: #tools #scope

The formatter interruption test compared executable path strings while `land()`
resolves the repository root. On macOS the temporary path may start with `/var`
and resolve through `/private/var`, so the mock never raised its intended
`KeyboardInterrupt`.

The mock now compares resolved paths and exercises both the original temporary
root and an explicit symlink. The symlink case reproduced the reported failure
on Linux before the correction. Both cases now interrupt after source replacement
and confirm that rollback restores the original bytes. All 13 worker convenience
tests pass; production landing behavior is unchanged.
