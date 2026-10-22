# decomp-progress

A Claude Code mod that puts a small `◔ 64.50%` button in the prompt footer, beside
the mode labels. Clicking it (or typing `/decomp`) opens a popup with the Boktai 3
decomp's progress, in three tabs:

* **Overview**: everything in `PROGRESS.md`: the headline, the clean (unpinned)
  share, drafts, then every section and category as a bar. Click a category to see
  its units, sorted by how much is left; ROM regions are at the bottom.
* **Treemap**: categories split into units, sized by code bytes (from decomp.dev's
  report) and shaded by bytes matched (green is complete). Hover a tile for numbers.
* **Recent PRs**: merged PRs with the matches and progress each one reports.

It reads `PROGRESS.md` from `main`, the GitHub API's list of closed PRs and the
decomp.dev page, refreshing every ten minutes (↻ refreshes now). Nothing is
written anywhere but the mod's own store.

## Install

At the prompt of a terminal session:

```
/plugin install decomp-progress --marketplace moozilla/boktai3-decomp
```

In the desktop app: click + next to the prompt box, then Plugins, Add marketplace
with `moozilla/boktai3-decomp`, then Add plugin.

## Developing

`claude plugin validate tools/claude-mod` and `claude plugin test tools/claude-mod`
check the manifest and run `tests/popup.test.tsx`. `claude --plugin-dir tools/claude-mod`
loads it from the checkout.
