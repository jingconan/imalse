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
#include "p2plinkmanager.h"
#include "designerscene.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"

#include <QTableWidgetItem>


namespace netanim {

P2PLinkManager::P2PLinkManager(void * itemMap)
    : QDialog(),
      m_itemMap(itemMap)
{
    m_p2pTableWidget = new QTableWidget;
    m_vLayout = new QVBoxLayout;
    m_applyButton = new QPushButton("Apply");
    m_applyButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_applyButton->setGeometry(0, 0, 5, 5);
    m_applyButton->adjustSize();
    connect(m_applyButton,SIGNAL(clicked()),this,SLOT(applySlot()));
    m_vLayout->addWidget(m_p2pTableWidget);
    m_vLayout->addWidget(m_applyButton);
    setLayout(m_vLayout);
    initP2PTableHeader();

}

void
P2PLinkManager::initP2PTableHeader()
{

    QStringList headerList;
    headerList << "Link Id"
               << "Node A"
               << "Node B"
               << "Data Rate"
               << "Delay"
               << "Ipv4 Address Base"
               << "Ipv4 Mask";
    m_p2pTableWidget->setColumnCount(headerList.count());
    m_p2pTableWidget->setHorizontalHeaderLabels(headerList);


}

QString
P2PLinkManager::getColumnName(int column)
{
    return m_p2pTableWidget->horizontalHeaderItem(column)->text();
}

void
P2PLinkManager::applySlot()
{
    for(int row = 0; row < m_p2pTableWidget->rowCount(); ++row)
    {

        for(int column = 0; column < m_p2pTableWidget->columnCount(); ++column)
        {
            QTableWidgetItem * item = m_p2pTableWidget->item(row, column);
            uint64_t linkItemId = m_p2pTableWidget->item(row, 0)->text().toULongLong();
            DesignerP2PLinkLine * linkItem = getP2PLinkLike(linkItemId);
            QString columnName = getColumnName(column);
            QString itemText = item->text();
            if(columnName == "Delay")
                linkItem->setDelay(itemText);
            else if(columnName == "Data Rate")
                linkItem->setDataRate(itemText);
            else if(columnName == "Ipv4 Address Base")
                linkItem->setIpv4Base(itemText);
            else if(columnName == "Ipv4 Mask")
                linkItem->setIpv4Mask(itemText);
        }

    }// for rows

}

void
P2PLinkManager::addRow(DesignerP2PLinkLine *linkItem)
{
    int row = m_p2pTableWidget->rowCount();
    m_p2pTableWidget->insertRow(row);
    int columnCount = m_p2pTableWidget->columnCount();
    for(int i = 0; i < columnCount; ++i)
    {
        QTableWidgetItem * wi = new QTableWidgetItem;
        m_p2pTableWidget->setItem(row, i, wi);
        if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Link Id" )
        {
            wi->setText(QString::number(linkItem->getDesignerItemId()));
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Node A")
        {
            wi->setText(QString::number(getNodeIdfromItemId(linkItem->getSrcNodeItemId())));
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Node B")
        {
            wi->setText(QString::number(getNodeIdfromItemId(linkItem->getDstNodeItemId())));
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Data Rate")
        {
            wi->setText(linkItem->getDataRate());
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Delay")
        {
            wi->setText(linkItem->getDelay());
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Ipv4 Address Base")
        {
            wi->setText(linkItem->getIpv4Base());
        }
        else if(m_p2pTableWidget->horizontalHeaderItem(i)->text() == "Ipv4 Mask")
        {
            wi->setText(linkItem->getIpv4Mask());
        }

    }
}

void
P2PLinkManager::populateTables()
{
    for(LinkItemIdSrcDstNodeItemIdPair_t::const_iterator i = m_linkNodes.begin();
        i != m_linkNodes.end();
        ++i)
    {
        DesignerP2PLinkLine * linkItem = getP2PLinkLike(i->first);
        addRow(linkItem);
    }
}

void
P2PLinkManager::showDialog()
{
    m_p2pTableWidget->clearContents();
    m_p2pTableWidget->setRowCount(0);
    populateTables();
    exec();

}

void
P2PLinkManager::addLinkNodePair(uint64_t linkId, uint64_t nodeA, uint64_t nodeB)
{
    P2PLinkManager::NodeItemIdPair_t pair(nodeA,nodeB);
    m_linkNodes[linkId] = pair;
}


uint32_t
P2PLinkManager::getNodeIdfromItemId(uint64_t itemId)
{
    DesignerScene::ItemIdItemMap_t * im = (DesignerScene::ItemIdItemMap_t *) (m_itemMap);
    DesignerNodeRect * nodeItem = dynamic_cast <DesignerNodeRect*> ((*im)[itemId]);
    return nodeItem->getNodeId();
}

P2PLinkManager::LinkItemIdNodeIdPairVector_t
P2PLinkManager::getPeerNodeId(uint64_t myNodeItemId)
{
    P2PLinkManager::LinkItemIdNodeIdPairVector_t results;

    for(P2PLinkManager::LinkItemIdSrcDstNodeItemIdPair_t::const_iterator i = m_linkNodes.begin();
        i != m_linkNodes.end();
        ++i)
    {
        uint64_t linkItemId = i->first;
        P2PLinkManager::NodeItemIdPair_t nodePair = i->second;
        LinkItemIdNodeIdPair_t ln;
        ln.first = linkItemId;
        if((nodePair.first == myNodeItemId))
        {
            ln.second = getNodeIdfromItemId(nodePair.second);
            results.push_back(ln);

        }
        else if(nodePair.second == myNodeItemId)
        {
            ln.second = getNodeIdfromItemId(nodePair.first);
            results.push_back(ln);

        }
    }
    return results;

}

DesignerP2PLinkLine *
P2PLinkManager::getP2PLinkLike(uint64_t itemId)
{
    DesignerScene::ItemIdItemMap_t * im = (DesignerScene::ItemIdItemMap_t *) (m_itemMap);
    DesignerP2PLinkLine * linkItem = dynamic_cast <DesignerP2PLinkLine*> ((*im)[itemId]);
    return linkItem;
}

void
P2PLinkManager::setLinkProperties(uint64_t linkItemId, QString dataRate, QString delay)
{
    DesignerP2PLinkLine * linkItem = getP2PLinkLike(linkItemId);
    linkItem->setDataRate(dataRate);
    linkItem->setDelay(delay);

}


void
P2PLinkManager::applySimulation()
{
    for(LinkItemIdSrcDstNodeItemIdPair_t::const_iterator i = m_linkNodes.begin();
        i != m_linkNodes.end();
        ++i)
    {
        DesignerP2PLinkLine * linkItem = getP2PLinkLike(i->first);
        applyLinkItem(linkItem);
    }
}

void
P2PLinkManager::applyLinkItem(DesignerP2PLinkLine *linkItem)
{
    ns3::Ptr <ns3::Node> srcNode = ns3::NodeList::GetNode(getNodeIdfromItemId(linkItem->getSrcNodeItemId()));
    ns3::Ptr <ns3::Node> dstNode = ns3::NodeList::GetNode(getNodeIdfromItemId(linkItem->getDstNodeItemId()));
    ns3::NodeContainer nodeContainer;
    nodeContainer.Add(srcNode);
    nodeContainer.Add(dstNode);
    ns3::PointToPointHelper p2pHelper;
    p2pHelper.SetDeviceAttribute("DataRate", ns3::StringValue (linkItem->getDataRate().toAscii().data()));
    p2pHelper.SetChannelAttribute("Delay", ns3::StringValue (linkItem->getDelay().toAscii().data()));

    ns3::NetDeviceContainer devices;
    devices = p2pHelper.Install (nodeContainer);

    ns3::Ipv4AddressHelper address;
    address.SetBase (linkItem->getIpv4Base().toAscii().data(),
                     linkItem->getIpv4Mask().toAscii().data());

    address.Assign (devices);

}

} // namespace netanim
