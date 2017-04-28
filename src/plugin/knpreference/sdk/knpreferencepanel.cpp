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
#include <QLabel>

#include "knthememanager.h"
#include "knpreferencepanel.h"

//Items
#include "items/knpreferencepanelitem.h"
#include "items/knpreferencepanelbooleanitem.h"
#include "items/knpreferencepanelfontitem.h"

#include <QDebug>

KNPreferencePanel::KNPreferencePanel(QWidget *parent) :
    QWidget(parent),
    m_titleFont(font()),
    m_mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this))
{
    //Set properties.
    setAutoFillBackground(true);
    setFixedWidth(522);
    //Set layout.
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    setLayout(m_mainLayout);
    //Get the palette.
    setPalette(knTheme->getPalette("PreferencePanel"));
    //Set the title font.
    m_titleFont.setPixelSize(14);
}

void KNPreferencePanel::setPanelBlocks(
        const QList<PreferencePanelBlock> &blocks)
{
    //Parse each block.
    for(auto block : blocks)
    {
        //Generate the label.
        QLabel *titleLabel=new QLabel(this);
        //Configure the label.
        titleLabel->setContentsMargins(20, 20, 20, 5);
        titleLabel->setText(block.blockName);
        titleLabel->setPalette(palette());
        titleLabel->setFont(m_titleFont);
        //Add the block title to layout.
        m_mainLayout->addWidget(titleLabel);
        //Loop and generate all the item.
        for(auto item : block.options)
        {
            //Generate the item according to its type.
            KNPreferencePanelItem *itemWidget=generateItem(item);
            //Check the item widget.
            if(!itemWidget)
            {
                //Go on to next item.
                continue;
            }
            //Add item to the layout.
            m_mainLayout->addWidget(itemWidget);
        }
    }
    //Add stretch.
    m_mainLayout->addStretch();
}

inline KNPreferencePanelItem *KNPreferencePanel::generateItem(
        const PreferencePanelOption &option)
{
    //Prepare the item.
    KNPreferencePanelItem *item=nullptr;
    //Generate the item.
    switch(option.type)
    {
    case TypeBoolean:
        item=new KNPreferencePanelBooleanItem(this);
        break;
    case TypeFont:
        item=new KNPreferencePanelFontItem(this);
        break;
    default:
        break;
    }
    //Set the option.
    item->setPreferenceOption(option);
    //Give back the item.
    return item;
}
