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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KNMUSICNOWPLAYINGDELEGATE_H
#define KNMUSICNOWPLAYINGDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The KNMusicNowPlayingDelegate class provides the list view delegate
 * for the main player's playlist.
 */
class KNMusicNowPlayingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNMusicNowPlayingDelegate widget.
     * \param parent The parent widget.
     */
    explicit KNMusicNowPlayingDelegate(QWidget *parent = 0);

    /*!
     * \brief Reimplemented from QStyledItemDelegate::paint().
     */
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;

    /*!
     * \brief Reimplemented from QStyledItemDelegate::sizeHint().
     */
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;

signals:

public slots:

private:
    inline QString textData(const QAbstractItemModel *model,
                            const QModelIndex &rowIndex,
                            const int &column) const;
};

#endif // KNMUSICNOWPLAYINGDELEGATE_H
