/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KT_REMOVESCRUBPLUGIN_H
#define KT_REMOVESCRUBPLUGIN_H

#include <interfaces/plugin.h>
#include <interfaces/torrentactivityinterface.h>

class QAction;

namespace kt
{
class RemoveScrubPlugin : public Plugin, public ViewListener
{
    Q_OBJECT
public:
    RemoveScrubPlugin(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~RemoveScrubPlugin() override;

    void load() override;
    void unload() override;
    QString parentPart() const override
    {
        return QStringLiteral("torrentactivity");
    }
    void currentTorrentChanged(bt::TorrentInterface *tc) override;

private Q_SLOTS:
    void removeAndScrub();

private:
    QList<bt::TorrentInterface *> selectedTorrents();

    QAction *remove_scrub_action;
};

}

#endif // KT_REMOVESCRUBPLUGIN_H
