#!/bin/bash
# Installs the built plugin for the Flatpak KTorrent (user level, no root):
# copies it into the app's own data folder and adds that folder to the plugin search path.
set -e
ROOT="$(dirname "$(cd "$(dirname "$0")" && pwd)")"
DEST="$HOME/.var/app/org.kde.ktorrent/plugins"
mkdir -p "$DEST/ktorrent_plugins"
cp "$ROOT/plugins/ktorrent_plugins/RemoveScrubPlugin.so" "$DEST/ktorrent_plugins/"
flatpak override --user "--env=QT_PLUGIN_PATH=$DEST:/app/lib/plugins:/usr/share/runtime/lib/plugins" org.kde.ktorrent
echo "Installed. Restart KTorrent to load it."
