/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "removescrubplugin.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMainWindow>
#include <KPluginFactory>

#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QItemSelectionModel>
#include <QTreeView>
#include <QUrl>

#include <interfaces/coreinterface.h>
#include <interfaces/guiinterface.h>
#include <interfaces/torrentinterface.h>
#include <torrent/queuemanager.h>

K_PLUGIN_CLASS_WITH_JSON(kt::RemoveScrubPlugin, "ktorrent_removescrub.json")

using namespace bt;
namespace kt
{
static const char ui_xml[] =
    "<?xml version=\"1.0\"?>\n"
    "<!DOCTYPE gui SYSTEM \"kpartgui.dtd\">\n"
    "<gui name=\"removescrubplugin\" version=\"1\">\n"
    "\t<Menu name=\"ViewMenu\">\n"
    "\t\t<Action name=\"remove_and_scrub\"/>\n"
    "\t</Menu>\n"
    "</gui>\n";

RemoveScrubPlugin::RemoveScrubPlugin(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : Plugin(parent, data, args)
{
    remove_scrub_action = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Torrent and Delete .torrent File"), this);
    connect(remove_scrub_action, &QAction::triggered, this, &RemoveScrubPlugin::removeAndScrub);
    actionCollection()->addAction(QStringLiteral("remove_and_scrub"), remove_scrub_action);
    setXML(QString::fromLatin1(ui_xml));
}

RemoveScrubPlugin::~RemoveScrubPlugin()
{
}

void RemoveScrubPlugin::load()
{
    TorrentActivityInterface *ta = getGUI()->getTorrentActivity();
    ta->addViewListener(this);
    currentTorrentChanged(ta->getCurrentTorrent());
}

void RemoveScrubPlugin::unload()
{
    TorrentActivityInterface *ta = getGUI()->getTorrentActivity();
    ta->removeViewListener(this);
}

void RemoveScrubPlugin::currentTorrentChanged(bt::TorrentInterface *tc)
{
    remove_scrub_action->setEnabled(tc != nullptr);
}

QList<bt::TorrentInterface *> RemoveScrubPlugin::selectedTorrents()
{
    QList<bt::TorrentInterface *> known;
    QueueManager *qman = getCore()->getQueueManager();
    for (auto i = qman->begin(); i != qman->end(); ++i) {
        known.append(*i);
    }

    // The torrent view is private to the ktorrent executable, so the selected rows are read
    // through the generic item view API. Each row's internal pointer is a ViewModel::Item whose
    // first member is the torrent. Anything that is not a torrent the core knows about is dropped.
    QList<bt::TorrentInterface *> sel;
    const QList<QTreeView *> views = getGUI()->getMainWindow()->findChildren<QTreeView *>();
    for (QTreeView *view : views) {
        if (qstrcmp(view->metaObject()->className(), "kt::View") != 0 || !view->selectionModel()) {
            continue;
        }

        const QModelIndexList rows = view->selectionModel()->selectedRows();
        for (const QModelIndex &idx : rows) {
            void *item = idx.internalPointer();
            if (!item) {
                continue;
            }

            bt::TorrentInterface *tc = *static_cast<bt::TorrentInterface **>(item);
            if (known.contains(tc) && !sel.contains(tc)) {
                sel.append(tc);
            }
        }
    }

    if (sel.isEmpty()) {
        bt::TorrentInterface *tc = getGUI()->getTorrentActivity()->getCurrentTorrent();
        if (tc && known.contains(tc)) {
            sel.append(tc);
        }
    }

    return sel;
}

void RemoveScrubPlugin::removeAndScrub()
{
    const QList<bt::TorrentInterface *> sel = selectedTorrents();
    int removed = 0;
    int scrubbed = 0;
    QStringList failed;

    for (bt::TorrentInterface *tc : sel) {
        // Collect everything needed before the removal, the core deletes tc.
        QString file;
        const QUrl url = tc->loadUrl();
        if (url.isLocalFile()) {
            const QFileInfo fi(url.toLocalFile());
            const QString tor_dir = QDir::cleanPath(tc->getTorDir()) + QLatin1Char('/');
            if (fi.isFile() && fi.suffix().compare(QLatin1String("torrent"), Qt::CaseInsensitive) == 0 && !fi.absoluteFilePath().startsWith(tor_dir)) {
                file = fi.absoluteFilePath();
            }
        }

        getCore()->remove(tc, false);
        removed++;

        if (!file.isEmpty()) {
            // QFile::remove unlinks the file directly, it does not go to the trash.
            if (QFile::remove(file)) {
                scrubbed++;
            } else {
                failed.append(file);
            }
        }
    }

    if (!failed.isEmpty()) {
        getGUI()->errorMsg(i18n("Could not delete the following torrent files:\n%1", failed.join(QLatin1Char('\n'))));
    }

    if (removed > 0) {
        getGUI()->getStatusBar()->message(i18n("Removed %1 torrent(s), deleted %2 .torrent file(s)", removed, scrubbed));
    }
}

}

#include "removescrubplugin.moc"

#include "moc_removescrubplugin.cpp"
