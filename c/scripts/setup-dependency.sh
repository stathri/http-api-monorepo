#!/usr/bin/env bash
set -euo pipefail

# Fetch the known-good CivetWeb version used by this project.
if [ -d third_party/civetweb/.git ]; then
    git -C third_party/civetweb fetch --depth 1 origin tag v1.16
    git -C third_party/civetweb checkout --detach refs/tags/v1.16
else
    git clone --depth 1 --branch v1.16 https://github.com/civetweb/civetweb.git third_party/civetweb
fi
