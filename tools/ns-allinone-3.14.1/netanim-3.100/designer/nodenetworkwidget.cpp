/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: John Abraham <john.abraham@gatech.edu>
 */

#include "nodenetworkwidget.h"
#include "designernoderect.h"
#include "p2plinkmanager.h"

#include <QtGui/QScrollBar>

namespace netanim {

NodeNetworkWidget::NodeNetworkWidget(void * item,
                                     void * linkMgr,
                                     QWidget * parent) :
    QWidget(parent),
    m_item(item),
    m_linkManager(linkMgr)

{
    m_vLayout = new QVBoxLayout;

    m_p2pTableWidget = new QTableWidget(0, 4, this);
    m_p2pTableWidget->setMinimumSize(10, 10);
    QStringList columnNames;
    columnNames<<"LinkItemId";
    columnNames<<"PeerNodeId";
    columnNames<<"DataRate";
    columnNames<<"Delay";
    m_p2pTableWidget->setHorizontalHeaderLabels(columnNames);
    m_vLayout->addWidget(m_p2pTableWidget);
    m_applyButton = new QPushButton("Apply");
    m_applyButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_applyButton->setGeometry(0, 0, 5, 5);
    m_applyButton->adjustSize();
    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(applySlot()));
    m_vLayout->addWidget(m_applyButton);
    setLayout(m_vLayout);
    show();
    addP2PDevices();
}

void
NodeNetworkWidget::applySlot()
{
    P2PLinkManager * linkManager = (P2PLinkManager *) m_linkManager;

    int rowCount = m_p2pTableWidget->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        QTableWidgetItem * linkItem = m_p2pTableWidget->item(i,0);
        QTableWidgetItem * dataRateItem = m_p2pTableWidget->item(i,2);
        QTableWidgetItem * delayItem = m_p2pTableWidget->item(i,3);
        linkManager->setLinkProperties(linkItem->text().toULongLong(),
                                       dataRateItem->text(),
                                       delayItem->text());

    }

}

void
NodeNetworkWidget::addP2PDevices()
{

    P2PLinkManager * linkManager = (P2PLinkManager *) m_linkManager;
    DesignerNodeRect * nodeItem = (DesignerNodeRect *) (m_item);
    if(!nodeItem)
        return;
    P2PLinkManager::LinkItemIdNodeIdPairVector_t linknodeVector = linkManager->getPeerNodeId(nodeItem->getDesignerItemId());
    for(int i = 0; i < linknodeVector.size(); i++)
    {
        P2PLinkManager::LinkItemIdNodeIdPair_t ln = linknodeVector[i];
        DesignerP2PLinkLine * linkItem = linkManager->getP2PLinkLike(ln.first);
        addP2PLink(ln.first, ln.second, linkItem->getDataRate(), linkItem->getDelay());
    }

}


void
NodeNetworkWidget::addP2PLink(uint64_t linkItemId, uint32_t peerId, QString dataRate, QString delay)
{
    int row = m_p2pTableWidget->rowCount();
    m_p2pTableWidget->insertRow(row);
    QTableWidgetItem * linkItem = new QTableWidgetItem;
    linkItem->setText(QString::number(linkItemId));
    linkItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    m_p2pTableWidget->setItem(row, 0, linkItem);
    QTableWidgetItem * peerItem = new QTableWidgetItem;
    peerItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    peerItem->setText(QString::number(peerId));
    m_p2pTableWidget->setItem(row, 1, peerItem);
    QTableWidgetItem * dataRateItem = new QTableWidgetItem;
    dataRateItem->setText(dataRate);
    m_p2pTableWidget->setItem(row, 2, dataRateItem);
    QTableWidgetItem * delayItem = new QTableWidgetItem;
    delayItem->setText(delay);
    m_p2pTableWidget->setItem(row, 3, delayItem);
    m_p2pTableWidget->setVisible(true);


}

} // namespace netanim
