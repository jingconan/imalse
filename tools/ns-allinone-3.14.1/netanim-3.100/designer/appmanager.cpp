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
#include "appmanager.h"

#include "ns3/log.h"
#include "ns3/fatal-error.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

namespace netanim {

NS_LOG_COMPONENT_DEFINE("AppManager");

uint64_t gAppId = 0;

AppManager::AppManager()
{
    addAvailableApps();
    m_appTableWidget = new QTableWidget;
    m_onOffTableWidget = new QTableWidget;
    m_packetSinkTableWidget = new QTableWidget;

    m_vLayout = new QVBoxLayout;
    m_applyButton = new QPushButton("Apply");
    m_applyButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_applyButton->setGeometry(0, 0, 5, 5);
    m_applyButton->adjustSize();
    connect(m_applyButton,SIGNAL(clicked()),this,SLOT(applySlot()));

    m_startTimeLabel = new QLabel("Start Time");
    m_startTimeLineEdit = new QLineEdit("0");

    m_stopTimeLabel = new QLabel("Stop Time");
    m_stopTimeLineEdit = new QLineEdit("0");


    m_vLayout->addWidget(m_onOffTableWidget);
    m_vLayout->addWidget(m_packetSinkTableWidget);
    m_vLayout->addWidget(m_applyButton);
    m_vLayout->addWidget(m_startTimeLabel);
    m_vLayout->addWidget(m_startTimeLineEdit);
    m_vLayout->addWidget(m_stopTimeLabel);
    m_vLayout->addWidget(m_stopTimeLineEdit);


    m_refreshTimer.setInterval(200);
    connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshSlot()));
    setLayout(m_vLayout);
    initOnOffTableHeader();
    initPacketSinkTableHeader();
}



void
AppManager::refreshSlot()
{

    int rowCount = m_packetSinkTableWidget->rowCount();
    for(int row = 0; row < rowCount; ++row)
    {
        QTableWidgetItem * wiAppId =  m_packetSinkTableWidget->item(row, 0);
        ns3::Ptr <ns3::PacketSink> packetSinkApp = m_packetSinkMap[wiAppId->text().toInt()];
        QTableWidgetItem * wiPacketSinkRx = m_packetSinkTableWidget->item(row, 3);
        wiPacketSinkRx->setText(QString::number(packetSinkApp->GetTotalRx()));
        ++row;
    }
    m_refreshTimer.start();
}


void
AppManager::initOnOffTableHeader()
{
    QStringList headerList;
    headerList << "App Id"
               << "Node Id"
               << "Type";
    m_onOffTableWidget->setColumnCount(headerList.count());
    m_onOffTableWidget->setHorizontalHeaderLabels(headerList);
}

void
AppManager::initPacketSinkTableHeader()
{
    QStringList headerList;
    headerList << "App Id"
               << "Node Id"
               << "Type"
               << "Total Rx";
    m_packetSinkTableWidget->setColumnCount(headerList.count());
    m_packetSinkTableWidget->setHorizontalHeaderLabels(headerList);

}

void
AppManager::initAppTableHeader()
{
    QStringList headerList;
    headerList << "App Id"
               << "Node Id"
               << "Type";
    m_appTableWidget->setColumnCount(headerList.count());
    m_appTableWidget->setHorizontalHeaderLabels(headerList);

}

void
AppManager::addAvailableApps()
{
    m_availableApps.push_back(OnOff);
    m_availableApps.push_back(PacketSink);

}

AppManager::AppTypeVector_t
AppManager::getAvailableApps()
{
    return m_availableApps;
}


QStringList
AppManager::getAppAttributeList(AppType_t appType)
{
    QStringList appAttributeList;
    switch (appType)
    {
        case OnOff:
        {
            appAttributeList<<"DataRate"
                            <<"PacketSize"
                            <<"Remote"
                            <<"OnTime"
                            <<"OffTime"
                            <<"MaxTime";
            break;
        }
        case PacketSink:
        {
            break;
        }
        default:
        {

        }

    }
    return appAttributeList;
}


QString
AppManager::appTypeToString(AppType_t type)
{
    switch(type)
    {
        case OnOff:
        {
            return "OnOff";
            break;
        }
        case PacketSink:
        {
            return "PacketSink";
            break;
        }
        default:
        {
            //NS_FATAL_ERROR("Unsupported app type");
        }

    }
    return "";
}

AppManager::AppType_t
AppManager::appStringToType(QString appString)
{
    if(appString == "OnOff")
        return OnOff;
    if(appString == "PacketSink")
        return PacketSink;
    NS_FATAL_ERROR("Invalid app");
    return InvalidApp;

}

void
AppManager::clearOnOffApps(uint32_t nodeId)
{
    QVector <uint64_t> appVector;
    for(AppManager::AppIdOnOffMap_t::const_iterator i = m_onOffMap.begin();
        i != m_onOffMap.end();
        ++i)
    {
        if(nodeId == getNodeIdFromAppId(i->first))
        {
            appVector.push_back(i->first);
        }
    }

    for(int i = 0; i < appVector.size(); ++i)
    {
        m_onOffMap.erase(appVector[i]);
    }

}

void
AppManager::clearPacketSinks(uint32_t nodeId)
{
    QVector <uint64_t> appVector;
    for(AppManager::AppIdPacketSinkMap_t::const_iterator i = m_packetSinkMap.begin();
        i != m_packetSinkMap.end();
        ++i)
    {
        if(nodeId == getNodeIdFromAppId(i->first))
        {
            appVector.push_back(i->first);
        }
    }

    for(int i = 0; i < appVector.size(); ++i)
    {
        m_packetSinkMap.erase(appVector[i]);
    }

}

void
AppManager::setOnOffVector(uint32_t nodeId, OnOffVector_t onOffVec)
{
    clearOnOffApps(nodeId);
    //qDebug(QString::number(nodeId).toAscii().data());
    for(OnOffVector_t::const_iterator i = onOffVec.begin();
        i != onOffVec.end();
        ++i)
    {
        m_appIdNodeIdMap[gAppId] = nodeId;
        m_onOffMap[gAppId++] = *i;
    }
}


void
AppManager::setPacketSinkVector(uint32_t nodeId, PacketSinkVector_t packetSinkVec)
{
    clearPacketSinks(nodeId);
    for(PacketSinkVector_t::const_iterator i = packetSinkVec.begin();
        i != packetSinkVec.end();
        ++i)
    {
        m_appIdNodeIdMap[gAppId] = nodeId;
        m_packetSinkMap[gAppId++] = *i;
    }
}

void
AppManager::addRow()
{
    int row = m_appTableWidget->rowCount();
    m_appTableWidget->insertRow(row);

}

void
AppManager::populateTables()
{
    int row = 0;
    for(AppIdOnOffMap_t::const_iterator i = m_onOffMap.begin();
        i != m_onOffMap.end();
        ++i)
    {
        m_onOffTableWidget->insertRow(row);
        QTableWidgetItem * wiAppId =
                new QTableWidgetItem;
        wiAppId->setText(QString::number(i->first));
        m_onOffTableWidget->setItem(row, 0, wiAppId);


        QTableWidgetItem * wiNodeId =
                new QTableWidgetItem;
        wiNodeId->setText(QString::number(m_appIdNodeIdMap[i->first]));
        m_onOffTableWidget->setItem(row, 1, wiNodeId);

        QTableWidgetItem * wiApptype =
                new QTableWidgetItem;
        wiApptype->setText("OnOff");
        m_onOffTableWidget->setItem(row, 2, wiApptype);
        ++row;
    }

    row = 0;
    for(AppIdPacketSinkMap_t::const_iterator i = m_packetSinkMap.begin();
        i != m_packetSinkMap.end();
        ++i)
    {
        m_packetSinkTableWidget->insertRow(row);
        QTableWidgetItem * wiAppId =
                new QTableWidgetItem;
        wiAppId->setText(QString::number(i->first));
        m_packetSinkTableWidget->setItem(row, 0, wiAppId);

        QTableWidgetItem * wiNodeId =
                new QTableWidgetItem;
        wiNodeId->setText(QString::number(m_appIdNodeIdMap[i->first]));
        m_packetSinkTableWidget->setItem(row, 1, wiNodeId);
        QTableWidgetItem * wiApptype =
                new QTableWidgetItem;
        wiApptype->setText("PacketSink");
        m_packetSinkTableWidget->setItem(row, 2, wiApptype);


        QTableWidgetItem * wiPacketSinkRx =
                new QTableWidgetItem;
        ns3::Ptr < ns3::PacketSink > packetSinkApp = i->second;
        wiPacketSinkRx->setText(QString::number(packetSinkApp->GetTotalRx()));
        m_packetSinkTableWidget->setItem(row, 3, wiPacketSinkRx);
        ++row;
    }

}

void
AppManager::showDialog()
{
    m_onOffTableWidget->clearContents();
    m_onOffTableWidget->setRowCount(0);
    m_packetSinkTableWidget->clearContents();
    m_packetSinkTableWidget->setRowCount(0);
    populateTables();
    m_refreshTimer.start();
    exec();
}

void
AppManager::applySlot()
{

}

uint32_t
AppManager::getNodeIdFromAppId(uint64_t appId)
{
    return m_appIdNodeIdMap[appId];
}

AppManager::AppIdOnOffMap_t
AppManager::getOnOffMap()
{
    return m_onOffMap;
}

AppManager::AppIdPacketSinkMap_t
AppManager::getPacketSinkMap()
{
    return m_packetSinkMap;
}

void
AppManager::install()
{
    for(AppIdOnOffMap_t::const_iterator i = m_onOffMap.begin();
        i != m_onOffMap.end();
        ++i)
    {
        ns3::Ptr <ns3::Node> node = ns3::NodeList::GetNode(getNodeIdFromAppId(i->first));
        ns3::Ptr <ns3::OnOffApplication> onOffApp = i->second;
        node->AddApplication(onOffApp);
        onOffApp->SetStartTime(ns3::Time(ns3::Seconds(1)));
        onOffApp->SetStopTime(ns3::Time(ns3::Seconds(20)));

    }


    for(AppIdPacketSinkMap_t::const_iterator i = m_packetSinkMap.begin();
        i != m_packetSinkMap.end();
        ++i)
    {
        ns3::Ptr <ns3::Node> node = ns3::NodeList::GetNode(getNodeIdFromAppId(i->first));
        ns3::Ptr <ns3::PacketSink> packetSinkApp = i->second;
        node->AddApplication(packetSinkApp);
        packetSinkApp->SetStartTime(ns3::Time(ns3::Seconds(0)));
        packetSinkApp->SetStopTime(ns3::Time(ns3::Seconds(20)));
    }

}

} //namespace netanim
