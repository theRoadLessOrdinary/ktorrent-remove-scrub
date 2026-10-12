#!/bin/bash
# Builds the plugin inside the Flatpak KTorrent sandbox (KDE SDK as runtime) and
# copies the result to plugins/ktorrent_plugins/ at the top of this repository.
# KTorrent does not ship its plugin headers, so matching source checkouts are
# fetched into upstream/ the first time this runs.
set -e
KT_TAG="${KT_TAG:-v26.04.3}"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(dirname "$HERE")"
mkdir -p "$ROOT/upstream"
for repo in ktorrent libktorrent; do
  if [ ! -d "$ROOT/upstream/$repo" ]; then
    git clone --depth 1 --branch "$KT_TAG" "https://invent.kde.org/network/$repo.git" "$ROOT/upstream/$repo"
  fi
done
flatpak run --devel --filesystem=host --command=bash org.kde.ktorrent -c "
  set -e
  cmake -S '$HERE' -B '$ROOT/build' -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DKTORRENT_SRC='$ROOT/upstream/ktorrent' -DLIBKTORRENT_SRC='$ROOT/upstream/libktorrent'
  cmake --build '$ROOT/build'
"
mkdir -p "$ROOT/plugins/ktorrent_plugins"
cp "$ROOT/build/bin/ktorrent_plugins/RemoveScrubPlugin.so" "$ROOT/plugins/ktorrent_plugins/"
echo "Built: $ROOT/plugins/ktorrent_plugins/RemoveScrubPlugin.so"
