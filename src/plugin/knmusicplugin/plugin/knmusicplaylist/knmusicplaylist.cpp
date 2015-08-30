/*
 * Copyright (C) Kreogist Dev Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <QSplitter>
#include <QFileDialog>

#include "kncategorytab.h"
#include "knemptystatewidget.h"
#include "knlocalemanager.h"
#include "knmessagebox.h"

// SDKs
#include "knmusicnowplayingbase.h"

// SDKs of playlists
#include "sdk/knmusicplaylistemptyhint.h"
#include "sdk/knmusicplaylistlist.h"
#include "sdk/knmusicplaylistlistmodel.h"
#include "sdk/knmusicplaylistviewer.h"
#include "sdk/knmusicplaylistmanager.h"
#include "sdk/knmusicplaylistmodel.h"

// Plugins
#include "plugin/knmusicplaylistitunesxmlparser/knmusicplaylistitunesxmlparser.h"
#include "plugin/knmusicplaylistxspfparser/knmusicplaylistxspfparser.h"
#include "plugin/knmusicplaylistwplparser/knmusicplaylistwplparser.h"
#include "plugin/knmusicplaylistttplparser/knmusicplaylistttplparser.h"
#include "plugin/knmusicplaylistm3uparser/knmusicplaylistm3uparser.h"
#include "plugin/knmusicplaylistm3uparser/knmusicplaylistm3u8parser.h"
#include "plugin/knmusicplaylistm3uparser/knmusicplaylistextm3uparser.h"

#include "knmusicglobal.h"

#include "knmusicplaylist.h"

#include <QDebug>

KNMusicPlaylist::KNMusicPlaylist(QWidget *parent) :
    KNMusicPlaylistBase(parent),
    m_tab(new KNCategoryTab(this)),
    m_container(new KNEmptyStateWidget(this)),
    m_playlistList(new KNMusicPlaylistList(this)),
    m_playlistViewer(new KNMusicPlaylistViewer(this)),
    m_playlistManager(new KNMusicPlaylistManager(this))
{
    //Configure the tab.
    m_tab->setIcon(QIcon(":/plugin/music/playlist/icon.png"));
    //Set the playlist folder path.
    m_playlistManager->setPlaylistDirPath(
                knMusicGlobal->musicLibPath()+"/Playlist");
    //Set the playlist list model to the playlist list.
    m_playlistList->setPlaylistList(m_playlistManager->playlistList());

    //Install the playlist parser.
    m_playlistManager->installPlaylistParser(
                                            new KNMusicPlaylistiTunesXMLParser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistXspfParser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistWplParser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistTtplParser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistM3uParser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistM3u8Parser);
    m_playlistManager->installPlaylistParser(new KNMusicPlaylistExtM3uParser);
    //Start running the engine.
    m_playlistManager->startParseEngine();

    //Link the GUI with che playlist manager.
    connect(m_playlistManager, &KNMusicPlaylistManager::requireShowContent,
            m_container, &KNEmptyStateWidget::showContentWidget);
    connect(m_playlistManager, &KNMusicPlaylistManager::requireHideContent,
            m_container, &KNEmptyStateWidget::showEmptyWidget);
    connect(m_playlistList, &KNMusicPlaylistList::requireCreatePlaylist,
            this, &KNMusicPlaylist::onActionCreatePlaylist);
    connect(m_playlistList, &KNMusicPlaylistList::requireImportPlaylists,
            this, &KNMusicPlaylist::onActionImportPlaylist);
    connect(m_playlistList, &KNMusicPlaylistList::requireExportPlaylist,
            this, &KNMusicPlaylist::onActionExportPlaylist);
    connect(m_playlistList, &KNMusicPlaylistList::requireRemovePlaylist,
            this, &KNMusicPlaylist::onActionRemovePlaylist);
    connect(m_playlistList, &KNMusicPlaylistList::requireShowPlaylist,
            [=](const QModelIndex &index)
            {
                m_playlistViewer->setPlaylist(
                            m_playlistManager->playlist(index));
            });

    //Generate the empty hint widget.
    KNMusicPlaylistEmptyHint *emptyHint=new KNMusicPlaylistEmptyHint(this);
    //Link the empty hint with the container.
    connect(emptyHint, &KNMusicPlaylistEmptyHint::requireAddPlaylist,
            this, &KNMusicPlaylist::onActionCreatePlaylist);
    connect(emptyHint, &KNMusicPlaylistEmptyHint::requireImportPlaylists,
            this, &KNMusicPlaylist::onActionImportPlaylist);
    //Set the empty widget and content widget.
    m_container->setEmptyWidget(emptyHint);
    //Get the splitter.
    QSplitter *contentContainer=generateSplitter();
    //Configure the splitter.
    contentContainer->setChildrenCollapsible(false);
    //Set the content widget as the splitter.
    m_container->setContentWidget(contentContainer);
    //Add the playlist list and playlist viewer to the splitter.
    contentContainer->addWidget(m_playlistList);
    contentContainer->addWidget(m_playlistViewer);
    //Set the default stretch factor.
    contentContainer->setStretchFactor(1, 1);

    //Link to retranslate.
    knI18n->link(this, &KNMusicPlaylist::retranslate);
    retranslate();
}

QAbstractButton *KNMusicPlaylist::tab()
{
    return m_tab;
}

void KNMusicPlaylist::showEvent(QShowEvent *event)
{
    //If the playlist list has never been load before, load it.
    if(!m_playlistManager->isPlaylistListLoaded())
    {
        //Load the playlist list.
        m_playlistManager->loadPlaylistList();
        //Get the playlist list temporarily.
        KNMusicPlaylistListModel *playlistList=
                m_playlistManager->playlistList();
        //Check if there's any playlist exist in the playlist list, select the
        //first one.
        if(playlistList->rowCount()>0)
        {
            //Select the first index.
            m_playlistList->showPlaylist(playlistList->index(0));
        }
    }
    //Do the original show event.
    KNMusicPlaylistBase::showEvent(event);
}

void KNMusicPlaylist::resizeEvent(QResizeEvent *event)
{
    //Resize the tab widget.
    KNMusicPlaylistBase::resizeEvent(event);
    //Resize the container.
    m_container->resize(size());
}

void KNMusicPlaylist::retranslate()
{
    //Update the tab title.
    m_tab->setText(tr("Playlist"));

    //Update the error message.
    m_cannotDeleteMessage=tr("Failed to delete the playlist file: \n"
                             "\n"
                             "%1");
}

void KNMusicPlaylist::onActionCreatePlaylist()
{
    //Generate a empty playlist model in the playlist manager, and get the index
    //of the model.
    QModelIndex playlistIndex=m_playlistManager->createPlaylist();
    //Show that playlist.
    m_playlistList->showPlaylist(playlistIndex);
    //Ask the list view to rename it.
    m_playlistList->renamePlaylist(playlistIndex);
    //Make the container switch to the content widget.
    m_container->showContentWidget();
}

void KNMusicPlaylist::onActionImportPlaylist()
{
    //Generate a file dialog.
    QFileDialog importPlaylists(this);
    //Set the file mode and the name filters.
#ifdef Q_OS_MACX
    importPlaylists.setWindowFlags(Qt::Sheet);
#endif
    importPlaylists.setFileMode(QFileDialog::ExistingFiles);
    importPlaylists.setNameFilters(m_playlistManager->playlistFilter());
    //Launch the selector.
    if(importPlaylists.exec() &&
            !importPlaylists.selectedFiles().isEmpty())
    {
        //Import all the selected files to the playlist manager.
        QModelIndex firstPlaylistIndex=
            m_playlistManager->importPlaylists(importPlaylists.selectedFiles());
        //Check whether the index is valid.
        if(firstPlaylistIndex.isValid())
        {
            //Show that playlist.
            m_playlistList->showPlaylist(firstPlaylistIndex);
            //Make the container switch to the content widget.
            m_container->showContentWidget();
        }
    }
}

void KNMusicPlaylist::onActionRemovePlaylist()
{
    //Get the current model from the viewer.
    if(!m_playlistViewer->playlist())
    {
        //Ignore when the playlist model is nullptr.
        return;
    }
    //Get the model which is going to be removed.
    KNMusicPlaylistModel *model=m_playlistViewer->playlist();
    //Check the now playing first.
    if(knMusicGlobal->nowPlaying())
    {
        //Ask the now playing to check the model which is going to be delete.
        knMusicGlobal->nowPlaying()->onActionModelRemoved(model);
    }
    //Remove the playlist file.
    //Check the existance of the file.
    if(QFile::exists(model->filePath()))
    {
        //Remove the file.
        if(!QFile::remove(model->filePath()))
        {
            //Show up the error message.
            KNMessageBox::information(
                        m_cannotDeleteMessage.arg(model->filePath()),
                        "Error",
                        Qt::AlignCenter);
            //Stop here.
            return;
        }
    }
    //Remove the playlist model.
    m_playlistManager->removePlaylist(model);
}

void KNMusicPlaylist::onActionExportPlaylist()
{
    //Get the current playlist model form the viewer.
    KNMusicPlaylistModel *playlist=m_playlistViewer->playlist();
    //Check the current playlist model.
    if(!playlist)
    {
        return;
    }
    //Generate a file dialog.
    QFileDialog exportPlaylist(this);
    //Set the file mode and the name filters.
#ifdef Q_OS_MACX
    exportPlaylist.setWindowFlags(Qt::Sheet);
#endif
    exportPlaylist.setFileMode(QFileDialog::AnyFile);
    exportPlaylist.setAcceptMode(QFileDialog::AcceptSave);
    //Set the default playlist file name.
    exportPlaylist.selectFile(playlist->title());
    //Get the export filter.
    QStringList fileFilter=m_playlistManager->playlistFilter();
    //Remove the all support one.
    fileFilter.removeFirst();
    //Set the export filter.
    exportPlaylist.setNameFilters(fileFilter);
    //Launch the export file.
    if(exportPlaylist.exec() &&
            !exportPlaylist.selectedFiles().isEmpty())
    {
        //Get the index of the selected filter.
        int selectedParserIndex=
                fileFilter.indexOf(exportPlaylist.selectedNameFilter());
        //Check the parser index.
        if(selectedParserIndex==-1)
        {
            //Something wrong happened, we cannot continue.
            return;
        }
        //Export the file.
        m_playlistManager->exportPlaylist(
                    playlist,
                    exportPlaylist.selectedFiles().first(),
                    selectedParserIndex);
    }
}

inline QSplitter *KNMusicPlaylist::generateSplitter()
{
    //Generate the splitter.
    QSplitter *splitter=new QSplitter(this);
    //Configure the splitter.
    splitter->setHandleWidth(0);
    //Return the splitter.
    return splitter;
}
