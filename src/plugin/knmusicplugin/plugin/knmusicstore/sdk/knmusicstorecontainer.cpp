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
 * along with this program; if not, write to the Free Software
Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <QBoxLayout>
#include <QScrollBar>

#include "knthememanager.h"
#include "knscrollarea.h"

#include "knmusicstorepagehome.h"
#include "knmusicstorepagealbum.h"
#include "knmusicstorepagesinglesong.h"
#include "knmusicstoreheader.h"
#include "knmusicstoreutil.h"

#include "knmusicstorecontainer.h"

#include <QDebug>

KNMusicStoreContainer::KNMusicStoreContainer(QWidget *parent) :
    QWidget(parent),
    m_pageContainer(new KNScrollArea(this)),
    m_contentWidth(KNMusicStoreUtil::maximumContentWidth())
{
    setObjectName("MusicStoreContainer");
    //Set the properties.
    setAutoFillBackground(true);
    setContentsMargins(0, 0, 0, 0);
    //Configure the page container.
    m_pageContainer->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_pageContainer->setVScrollBarTopMargin(KNMusicStoreUtil::headerHeight());
    //Configure the scroll bar.
    QScrollBar *containerScrollBar=m_pageContainer->vScrollBar();
    containerScrollBar->setObjectName("MusicStoreScrollBar");
    knTheme->registerWidget(containerScrollBar);
    containerScrollBar=m_pageContainer->hScrollBar();
    containerScrollBar->setObjectName("MusicStoreScrollBar");
    knTheme->registerWidget(containerScrollBar);
    //Update the page container object name.
    m_pageContainer->updateObjectName("MusicStorePageContainer");
    //Initial the pages.
    m_pages[PageHome]=new KNMusicStorePageHome(m_pageContainer);
    m_pages[PageSingleSong]=new KNMusicStorePageSingleSong(m_pageContainer);
    m_pages[PageAlbum]=new KNMusicStorePageAlbum(m_pageContainer);
    //Configure all widgets.
    configurePage(m_pages[PageHome]);
    configurePage(m_pages[PageSingleSong]);
    configurePage(m_pages[PageAlbum]);

    //Register the widget.
    knTheme->registerWidget(this);
}

KNMusicStorePage *KNMusicStoreContainer::page(int pageIndex)
{
    return m_pages[pageIndex];
}

void KNMusicStoreContainer::refresh()
{
    //Check the current index.
    if(m_pageContainer->widget())
    {
        //Find the widget in the array.
        for(int i=0; i<PagesCount; ++i)
        {
            //Check the page in the array.
            if(m_pageContainer->widget()==m_pages[i])
            {
                //Get the pages.
                showPageIndex(i);
                //Mission complete.
                return;
            }
        }
    }
    //No widget, require to show the home page.
    emit requireShowHome();
}

void KNMusicStoreContainer::resizeEvent(QResizeEvent *event)
{
    //Resize the widget.
    QWidget::resizeEvent(event);
    //Get current width.
    int contentWidth=width();
    //Calculate the content width.
    m_contentWidth=(contentWidth>KNMusicStoreUtil::maximumContentWidth())?
                        KNMusicStoreUtil::maximumContentWidth():
                        contentWidth;
    //Update the page container width.
    m_pageContainer->resize(size());
    //Update the page widget size.
    updatePageWidth(static_cast<KNMusicStorePage *>(m_pageContainer->widget()));
}

void KNMusicStoreContainer::showPage(KNMusicStorePage *pageWidget)
{
    //Check the page container.
    if(m_pageContainer->widget())
    {
        //Check those two widget.
        if(m_pageContainer->widget()==pageWidget)
        {
            //No need to change.
            return;
        }
        //Clear the widget.
        KNMusicStorePage *originalPageWidget=
                static_cast<KNMusicStorePage *>(m_pageContainer->takeWidget());
        //Hide the page widget, set the page parent to this.
        originalPageWidget->setParent(this);
        //Disconnect the previous page resize signal.
        disconnect(originalPageWidget, &KNMusicStorePage::requireUpdateHeight,
                   0, 0);
    }
    //Set the container to manage page widget.
    m_pageContainer->setWidget(pageWidget);
    //Emit the page change signal.
    emit currentPageChanged();
    //Update the page widget size.
    updatePageWidth(pageWidget);
}

void KNMusicStoreContainer::onShowPage()
{
    //The check the sender, cast it as page widget.
    showPage(static_cast<KNMusicStorePage *>(sender()));
}

void KNMusicStoreContainer::showPageIndex(int index)
{
    //Get the page.
    KNMusicStorePage *page=m_pages[index];
    //Check the index.
    switch (index)
    {
    case PageHome:
        //Fetch home information.
        emit requireShowHome();
        break;
    case PageAlbum:
        //Fetch the album information.
        emit requireShowAlbum(page->backendName(), page->metadata());
        break;
    case PageSingleSong:
        //Fetch the single song information.
        emit requireShowSingleSong(page->backendName(), page->metadata());
        break;
    default:
        break;
    }
}

void KNMusicStoreContainer::onShowAlbum(const QString &metadata)
{
    //The check the sender, cast it as page widget.
    KNMusicStorePage *pageWidget=static_cast<KNMusicStorePage *>(sender());
    //Resent the signal with name of backend.
    emit requireShowAlbum(pageWidget->backendName(),
                          metadata);
}

void KNMusicStoreContainer::onShowSingleSong(const QString &metadata)
{
    //The check the sender, cast it as page widget.
    KNMusicStorePage *pageWidget=static_cast<KNMusicStorePage *>(sender());
    //Resent the signal with name of backend.
    emit requireShowSingleSong(pageWidget->backendName(),
                               metadata);
}

void KNMusicStoreContainer::onUpdatePageWidth()
{
    //Recast the sender as the page, update its width and height.
    updatePageWidth(static_cast<KNMusicStorePage *>(sender()));
}

inline void KNMusicStoreContainer::updatePageWidth(KNMusicStorePage *pageWidget)
{
    //Check the page widget.
    if(pageWidget==nullptr)
    {
        //Ignore the page widget.
        return;
    }
    //Resize the page widget.
    pageWidget->setFixedWidth(m_contentWidth);
    //Check the size hint.
    pageWidget->setFixedHeight(qMax(height(),
                                    pageWidget->sizeHint().height()));
}

inline void KNMusicStoreContainer::configurePage(KNMusicStorePage *pageWidget)
{
    //Link the page signals.
    connect(pageWidget, &KNMusicStorePage::requireShowPage,
            this, &KNMusicStoreContainer::onShowPage);
    ;
    connect(pageWidget, &KNMusicStorePage::requireShowAlbum,
            this, &KNMusicStoreContainer::onShowAlbum);
    connect(pageWidget, &KNMusicStorePage::requireShowSingleSong,
            this, &KNMusicStoreContainer::onShowSingleSong);
    connect(pageWidget, &KNMusicStorePage::requireSetNavigatorItem,
            this, &KNMusicStoreContainer::requireSetNavigatorItem);
    connect(pageWidget, &KNMusicStorePage::requireDownload,
            this, &KNMusicStoreContainer::requireDownload);
    //Link the resize signals.
    connect(pageWidget, &KNMusicStorePage::requireUpdateHeight,
            this, &KNMusicStoreContainer::onUpdatePageWidth);
    //Hide the page.
    pageWidget->hide();
}
