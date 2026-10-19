#!/bin/sh
# Cloud sessions only: set this checkout up from the environment's cached base
# (tools/cloud_setup.sh --session; docs/CLOUD.md). Returns in seconds.
[ "$CLAUDE_CODE_REMOTE" = "true" ] || exit 0
exec sh "${CLAUDE_PROJECT_DIR:-.}/tools/cloud_setup.sh" --session
