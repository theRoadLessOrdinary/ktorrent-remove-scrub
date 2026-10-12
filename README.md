# KTorrent: Remove Torrent and Delete .torrent File

A small KTorrent plugin that adds one item to the torrent right-click menu:

**Remove Torrent and Delete .torrent File**

It does two things in one click:

1. Removes the selected torrent (or torrents) from the KTorrent list.
2. Permanently deletes the `.torrent` file each one was loaded from. The file is unlinked directly. It does not go to the trash.

The downloaded data is never touched.

## Why

KTorrent's own "Remove" takes the torrent off the list but leaves the original `.torrent` file sitting in your downloads folder. After a while that folder fills up with files you no longer need. This plugin cleans up both in one step.

## Details

- Works on a single torrent or a multi-selection.
- No confirmation dialog. The status bar reports how many torrents were removed and how many files were deleted.
- Only a local file ending in `.torrent` is deleted. A torrent added from a magnet link or a web URL has no local file, so it is only removed from the list.
- If a file cannot be deleted, an error box lists it.

## Requirements

This was written for and tested with the **Flatpak** build of KTorrent (`org.kde.ktorrent`) 26.04.3 on the KDE 6.11 runtime.

To build you need:

- The KTorrent Flatpak
- The matching KDE SDK: `flatpak install org.kde.Sdk//6.11`
- `git`

## Build and install

```
plugin/build.sh
plugin/install.sh
```

Then restart KTorrent.

`build.sh` fetches the KTorrent and libktorrent sources for the matching release into `upstream/` (headers only, KTorrent does not install its plugin headers), then compiles the plugin inside the Flatpak sandbox.

`install.sh` copies the plugin to `~/.var/app/org.kde.ktorrent/plugins/ktorrent_plugins/` and adds that folder to the app's plugin search path with a user-level `flatpak override`. No root needed.

The plugin is on by default. You can switch it off under Settings > Configure KTorrent > Plugins, where it is listed as "Remove and Delete Torrent File".

## After a KTorrent update

The plugin is compiled against one specific KTorrent release. When the Flatpak moves to a new release, the menu item may disappear until you rebuild:

1. Delete `upstream/` and `build/`.
2. Update the version numbers in `plugin/stubs/libktorrent_version.h` and `plugin/CMakeLists.txt`.
3. Run `KT_TAG=v<new version> plugin/build.sh`, then `plugin/install.sh`.

## Building for a distro package instead of Flatpak

Not tested, but the CMake project takes `KTORRENT_SRC`, `LIBKTORRENT_SRC` and `KTORRENT_LIBDIR`, so it can be pointed at a system install. The built `.so` then goes in your Qt plugin path under `ktorrent_plugins/`.

## Uninstall

```
rm ~/.var/app/org.kde.ktorrent/plugins/ktorrent_plugins/RemoveScrubPlugin.so
flatpak override --user --unset-env=QT_PLUGIN_PATH org.kde.ktorrent
```

## A note on how multi-select works

KTorrent's plugin interface only exposes the current torrent, not the full selection. To support multi-select, the plugin reads the selected rows from the torrent view and takes the torrent pointer out of each row's model item. Every pointer is checked against the list of torrents the core knows about before anything is done with it. If a future KTorrent changes that internal layout, the plugin falls back to acting on the current torrent only.

## License

GPL-2.0-or-later, the same as KTorrent.

By Bill in KCMO.
