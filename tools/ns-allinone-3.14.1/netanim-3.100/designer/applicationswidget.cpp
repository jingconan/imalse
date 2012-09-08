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

#include "applicationswidget.h"
#include "appmanager.h"
#include "designernoderect.h"
#include "attributerandomvariablewidget.h"
#include "attributeipv4addrportwidget.h"
#include "attributeprotocolwidget.h"
#include "debug/xdebug.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/type-id.h"

#include <QHeaderView>


namespace netanim {

Applicationswidget::Applicationswidget(void * nodeItem, void * appMgr, QWidget * parent):
    QWidget(parent),
    m_nodeItem(nodeItem),
    m_appManager(appMgr)
{
    setNodeId();
    m_vLayout = new QVBoxLayout;
    m_supportedAppsComboBox = new QComboBox;
    addSupportedApps();
    m_supportedAppsComboBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_supportedAppsComboBox->setFixedSize(150,20);
    m_supportedAppsComboBox->setToolTip("Select an application");

    m_addAppHLayout = new QHBoxLayout;
    m_addAppHLayout->addWidget(m_supportedAppsComboBox);
    m_addAppButton = new QPushButton("+");
    m_addAppHLayout->addWidget(m_addAppButton);
    m_addAppButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_addAppButton->setFixedSize(20,20);
    m_addAppButton->setToolTip("Add an application");
    connect(m_addAppButton,SIGNAL(clicked()),this,SLOT(addAppSlot()));

    m_onOffTableWidget = new QTableWidget(0, 0);
    m_onOffTableWidget->setMinimumSize(600, 0);
    initOnOffTable();


    m_packetSinkTableWidget = new QTableWidget(0, 0);
    m_packetSinkTableWidget->setMinimumSize(600, 0);
    initPacketSinkTable();

    m_vLayout->addLayout(m_addAppHLayout);

    m_onOffLabel = new QLabel("OnOff Applications");

    m_vLayout->addWidget(m_onOffLabel);
    m_vLayout->addWidget(m_onOffTableWidget);

    m_packetSinkLabel = new QLabel("PacketSink Applications");
    m_vLayout->addWidget(m_packetSinkLabel);
    m_vLayout->addWidget(m_packetSinkTableWidget);

    m_applyButton = new QPushButton("Apply");
    m_applyButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_applyButton->setGeometry(0, 0, 5, 5);
    m_applyButton->adjustSize();
    connect(m_applyButton,SIGNAL(clicked()),this,SLOT(applySlot()));
    m_vLayout->addWidget(m_applyButton);
    setLayout(m_vLayout);

    populateAppTables();

}

Applicationswidget::~Applicationswidget()
{
    if(m_supportedAppsComboBox)
        delete m_supportedAppsComboBox;
    if(m_vLayout)
        delete m_vLayout;

    for (int i = 0; i < m_objectsToDelete.size(); i++)
    {
        QPointer <QObject> obj = m_objectsToDelete[i];
        if(!obj.isNull())
        {
            delete obj;
        }

    }
}


void
Applicationswidget::populateAppTables()
{
    for(int i = 0; i < m_supportedAppsComboBox->count(); i++)
    {
        QString appType = m_supportedAppsComboBox->itemText(i);
        if(appType == "OnOff")
        {
            populateOnOffTable();
        }
        else if(appType == "PacketSink")
        {
            populatePacketSinkTable();
        }

    }
}

void
Applicationswidget::addToObjectsToDelete(QWidget *widget)
{
    QPointer <QObject> obj = dynamic_cast <QObject *> (widget);
    m_objectsToDelete.push_back(obj);
}


void
Applicationswidget::addPacketSinkToTable(ns3::Ptr<ns3::PacketSink> packetSinkApp)
{
    int row = m_packetSinkTableWidget->rowCount();
    m_packetSinkTableWidget->insertRow(row);
    ns3::TypeId packetTypeId = ns3::PacketSink::GetTypeId();
    uint32_t attributeCount = packetTypeId.GetAttributeN();
    for(uint32_t column = 0; column < attributeCount; ++column)
    {
        QString attrName = QString(packetTypeId.GetAttribute(column).name.c_str());
        std::string attrString = std::string(attrName.toAscii().data());
        if(attrString == "Local")
        {
            ns3::AddressValue av;
            packetSinkApp->GetAttributeFailSafe(attrString, av);
            ns3::InetSocketAddress ia = ns3::InetSocketAddress::ConvertFrom(av.Get());
            QPointer <AttributeIpv4AddrPortWidget> wi =
                    new AttributeIpv4AddrPortWidget;
            std::ostringstream os;
            os << ia.GetIpv4();
            wi->setIpv4Addr(QString(os.str().c_str()));
            wi->setPort(QString::number(ia.GetPort()));
            addToObjectsToDelete(wi);
            m_packetSinkTableWidget->setCellWidget(row, column, wi);

        }
        else if(attrString == "Protocol")
        {
            ns3::TypeIdValue tv;
            packetSinkApp->GetAttributeFailSafe(attrString, tv);

            AttributeProtocolWidget * wi =
                    new AttributeProtocolWidget;
            addToObjectsToDelete(wi);
            m_packetSinkTableWidget->setCellWidget(row, column, wi);
            wi->setTypeIdValue(tv);
        }

    }


}

void
Applicationswidget::addOnOffAppToTable(ns3::Ptr<ns3::OnOffApplication> onOffApp)
{

    int row = m_onOffTableWidget->rowCount();
    m_onOffTableWidget->insertRow(row);
    ns3::TypeId onOffTypeId = ns3::OnOffApplication::GetTypeId();
    uint32_t attributeCount = onOffTypeId.GetAttributeN();
    for(uint32_t column = 0; column < attributeCount; ++column)
    {
        QString attrName = QString(onOffTypeId.GetAttribute(column).name.c_str());
        std::string attrString = std::string(attrName.toAscii().data());
        if(attrString == "DataRate")
        {
            ns3::DataRateValue drv;
            onOffApp->GetAttributeFailSafe(attrString, drv);
            std::ostringstream os;
            os << drv.Get();
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText(QString(os.str().c_str()));
            m_onOffTableWidget->setItem(row, column, wi);
        }
        else if(attrString == "PacketSize")
        {
            ns3::UintegerValue uv;
            onOffApp->GetAttributeFailSafe(attrString, uv);
            std::ostringstream os;
            os << uv.Get();
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText(QString(os.str().c_str()));
            m_onOffTableWidget->setItem(row, column, wi);

        }
        else if(attrString == "Remote")
        {
            ns3::AddressValue av;
            onOffApp->GetAttributeFailSafe(attrString, av);
            ns3::InetSocketAddress ia = ns3::InetSocketAddress::ConvertFrom(av.Get());
            QPointer <AttributeIpv4AddrPortWidget> wi =
                    new AttributeIpv4AddrPortWidget;
            std::ostringstream os;
            os << ia.GetIpv4();
            wi->setIpv4Addr(QString(os.str().c_str()));
            wi->setPort(QString::number(ia.GetPort()));
            addToObjectsToDelete(wi);
            m_onOffTableWidget->setCellWidget(row, column, wi);

        }
        else if((attrString == "OnTime") || (attrString == "OffTime"))
        {
            ns3::RandomVariableValue rv;
            onOffApp->GetAttributeFailSafe(attrString, rv);
            AttributeRandomvariableWidget * wi =
                    new AttributeRandomvariableWidget;
            addToObjectsToDelete(wi);
            m_onOffTableWidget->setCellWidget(row, column, wi);
        }
        else if(attrString == "MaxBytes")
        {
            ns3::UintegerValue uv;
            onOffApp->GetAttributeFailSafe(attrString, uv);
            std::ostringstream os;
            os << uv.Get();
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText(QString(os.str().c_str()));
            m_onOffTableWidget->setItem(row, column, wi);

        }
        else if(attrString == "Protocol")
        {
            ns3::TypeIdValue tv;
            onOffApp->GetAttributeFailSafe(attrString, tv);

            AttributeProtocolWidget * wi =
                    new AttributeProtocolWidget;
            addToObjectsToDelete(wi);
            m_onOffTableWidget->setCellWidget(row, column, wi);
            wi->setTypeIdValue(tv);
        }



    }
}

void
Applicationswidget::populateOnOffTable()
{
    AppManager * appManager = ((AppManager *) m_appManager);
    AppManager::AppIdOnOffMap_t onOffMap =
            appManager->getOnOffMap();

    for(AppManager::AppIdOnOffMap_t::const_iterator i = onOffMap.begin();
        i != onOffMap.end();
        ++i)
    {
        if(m_nodeId == appManager->getNodeIdFromAppId(i->first))
        addOnOffAppToTable(i->second);
    }

}

void
Applicationswidget::populatePacketSinkTable()
{
    AppManager * appManager = ((AppManager *) m_appManager);
    AppManager::AppIdPacketSinkMap_t packetSinkMap =
            appManager->getPacketSinkMap();

    for (AppManager::AppIdPacketSinkMap_t::const_iterator i = packetSinkMap.begin();
         i != packetSinkMap.end();
         ++i)
    {
        if(m_nodeId == appManager->getNodeIdFromAppId(i->first))
        addPacketSinkToTable(i->second);
    }

}


void
Applicationswidget::applyPacketSink()
{

    AppManager::PacketSinkVector_t packetSinkVector;
    int rowCount = m_packetSinkTableWidget->rowCount();
    for(int row = 0; row < rowCount; ++row)
    {
        ns3::Ptr <ns3::PacketSink> packetSinkApp = ns3::CreateObject <ns3::PacketSink> ();
        ns3::TypeId packetSinkTypeId = ns3::PacketSink::GetTypeId();
        uint32_t attributeCount = packetSinkTypeId.GetAttributeN();
        for(uint32_t column = 0; column < attributeCount; ++column)
        {
            QString attrString = QString(packetSinkTypeId.GetAttribute(column).name.c_str());
            if(attrString == "Local")
            {
                AttributeIpv4AddrPortWidget * addressWidget =
                        dynamic_cast <AttributeIpv4AddrPortWidget *>
                        (m_packetSinkTableWidget->cellWidget(row, column));
                packetSinkApp->SetAttributeFailSafe("Local",
                                               addressWidget->getAddressValue());
            }
            else if(attrString == "Protocol")
            {
                AttributeProtocolWidget * protocolWidget =
                        dynamic_cast <AttributeProtocolWidget *>
                        (m_packetSinkTableWidget->cellWidget(row, column));
                packetSinkApp->SetAttributeFailSafe("Protocol",
                                               protocolWidget->getTypeIdValue());
            }

        } //for columns
        packetSinkVector.push_back(packetSinkApp);
    } //for rows
    if(packetSinkVector.size())
    {
        AppManager * appMgr = (AppManager *) m_appManager;
        appMgr->setPacketSinkVector(m_nodeId, packetSinkVector);
    }

}

void
Applicationswidget::applyOnOff()
{
    AppManager::OnOffVector_t onOffAppVector;
    int rowCount = m_onOffTableWidget->rowCount();
    for(int row = 0; row < rowCount; ++row)
    {
        ns3::Ptr <ns3::OnOffApplication> onOffApp = ns3::CreateObject <ns3::OnOffApplication> ();
        ns3::TypeId onOffTypeId = ns3::OnOffApplication::GetTypeId();
        uint32_t attributeCount = onOffTypeId.GetAttributeN();
        for(uint32_t column = 0; column < attributeCount; ++column)
        {
            QString attrName = QString(onOffTypeId.GetAttribute(column).name.c_str());
            if(attrName == "DataRate")
            {
                QString dr = m_onOffTableWidget->item(row, column)->text();
                onOffApp->SetAttributeFailSafe("DataRate",
                                               ns3::DataRateValue(ns3::DataRate(dr.toAscii().data())));


            }
            if(attrName == "PacketSize")
            {
                QString ps = m_onOffTableWidget->item(row, column)->text();
                onOffApp->SetAttributeFailSafe("PacketSize",
                                               ns3::UintegerValue(ps.toUInt()));

            }
            if(attrName == "Remote")
            {
                AttributeIpv4AddrPortWidget * addressWidget =
                        dynamic_cast <AttributeIpv4AddrPortWidget *>
                        (m_onOffTableWidget->cellWidget(row, column));
                onOffApp->SetAttributeFailSafe("Remote",
                                               addressWidget->getAddressValue());

            }
            if((attrName == "OnTime") || (attrName == "OffTime"))
            {
                AttributeRandomvariableWidget * rvWidget =
                        dynamic_cast <AttributeRandomvariableWidget *>
                        (m_onOffTableWidget->cellWidget(row, column));
                onOffApp->SetAttributeFailSafe(attrName.toAscii().data(),
                                               rvWidget->getRandomVariableValue());

            }
            if(attrName == "MaxBytes")
            {
                QString mb = m_onOffTableWidget->item(row, column)->text();
                onOffApp->SetAttributeFailSafe("MaxBytes",
                                               ns3::UintegerValue(mb.toUInt()));
            }
            if(attrName == "Protocol")
            {
                AttributeProtocolWidget * protocolWidget =
                        dynamic_cast <AttributeProtocolWidget *>
                        (m_onOffTableWidget->cellWidget(row, column));
                onOffApp->SetAttributeFailSafe("Protocol",
                                               protocolWidget->getTypeIdValue());


            }
        } // attribute for loop
        onOffAppVector.push_back(onOffApp);

    } // row for loop
    if(onOffAppVector.size())
    {
        AppManager * appMgr = (AppManager *) m_appManager;
        appMgr->setOnOffVector(m_nodeId, onOffAppVector);
    }

}

void
Applicationswidget::applySlot()
{

    applyOnOff();
    applyPacketSink();


}

void
Applicationswidget::initOnOffTable()
{
    ns3::TypeId onOffTypeId = ns3::OnOffApplication::GetTypeId();
    uint32_t attributeCount = onOffTypeId.GetAttributeN();
    m_onOffTableWidget->setColumnCount(attributeCount);
    QStringList attributeNames;
    for(uint32_t i = 0; i < attributeCount; i++)
    {
        attributeNames << QString(onOffTypeId.GetAttribute(i).name.c_str());
    }
    m_onOffTableWidget->setHorizontalHeaderLabels(attributeNames);

}


void
Applicationswidget::initPacketSinkTable()
{
    ns3::TypeId packetSinkTypeId = ns3::PacketSink::GetTypeId();
    uint32_t attributeCount = packetSinkTypeId.GetAttributeN();
    m_packetSinkTableWidget->setColumnCount(attributeCount);
    QStringList attributeNames;
    for(uint32_t i = 0; i < attributeCount; i++)
    {
        attributeNames << QString(packetSinkTypeId.GetAttribute(i).name.c_str());
    }
    m_packetSinkTableWidget->setHorizontalHeaderLabels(attributeNames);

}


void
Applicationswidget::addSupportedApps()
{
    AppManager * appMgr = (AppManager *) m_appManager;
    AppManager::AppTypeVector_t appVector = appMgr->getAvailableApps();
    for (AppManager::AppTypeVector_t::const_iterator i = appVector.begin();
         i != appVector.end();
         ++i)
    {
        m_supportedAppsComboBox->addItem(AppManager::appTypeToString(*i));

    }

}

void
Applicationswidget::addOnOffRow()
{
    int row = m_onOffTableWidget->rowCount();
    m_onOffTableWidget->insertRow(row);

    ns3::TypeId onOffTypeId = ns3::OnOffApplication::GetTypeId();
    uint32_t attributeCount = onOffTypeId.GetAttributeN();
    for(uint32_t column = 0; column < attributeCount; ++column)
    {
        QString attrName = QString(onOffTypeId.GetAttribute(column).name.c_str());
        if(attrName == "Remote")
        {
             QPointer <AttributeIpv4AddrPortWidget> ipv4AddrPortWidget =
                    new AttributeIpv4AddrPortWidget;
             ipv4AddrPortWidget->setIpv4Addr("10.1.1.2");
             ipv4AddrPortWidget->setPort("5000");
             addToObjectsToDelete(ipv4AddrPortWidget);
             m_onOffTableWidget->setCellWidget(row, column,
                                               qobject_cast <AttributeIpv4AddrPortWidget *>
                                               (ipv4AddrPortWidget));
        }
        else if((attrName == "OnTime") || (attrName == "OffTime"))
        {
            QPointer <QObject> onOffRVWidget =
                    new AttributeRandomvariableWidget;
            m_objectsToDelete.push_back(onOffRVWidget);
            m_onOffTableWidget->setCellWidget(row, column,
                                              qobject_cast <AttributeRandomvariableWidget *>
                                              (onOffRVWidget));

        }
        else if(attrName == "Protocol")
        {
            QPointer <QObject> protocolWidget =
                    new AttributeProtocolWidget;
            m_objectsToDelete.push_back(protocolWidget);
            m_onOffTableWidget->setCellWidget(row, column,
                                                qobject_cast <AttributeProtocolWidget *>
                                                (protocolWidget));

        }
        else if(attrName == "DataRate")
        {
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText("5Mbps");
            m_onOffTableWidget->setItem(row, column, wi);
        }
        else if(attrName == "PacketSize")
        {
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText("512");
            m_onOffTableWidget->setItem(row, column, wi);

        }
        else if(attrName == "MaxBytes")
        {
            QTableWidgetItem * wi = new QTableWidgetItem;
            wi->setText("2000000");
            m_onOffTableWidget->setItem(row, column, wi);

        }

    }


}

void
Applicationswidget::addPacketSinkRow()
{
    int row = m_packetSinkTableWidget->rowCount();
    m_packetSinkTableWidget->insertRow(row);
    ns3::TypeId packetSinkTypeId = ns3::PacketSink::GetTypeId();
    uint32_t attributeCount = packetSinkTypeId.GetAttributeN();
    for(uint32_t column = 0; column < attributeCount; ++column)
    {
        std::string attrString = packetSinkTypeId.GetAttribute(column).name;
        if(attrString == "Local")
        {
            QPointer <AttributeIpv4AddrPortWidget> ipv4AddrPortWidget =
                   new AttributeIpv4AddrPortWidget;
            ipv4AddrPortWidget->setIpv4Addr("10.1.1.2");
            ipv4AddrPortWidget->setPort("5000");
            addToObjectsToDelete(ipv4AddrPortWidget);
            m_packetSinkTableWidget->setCellWidget(row, column,
                                              qobject_cast <AttributeIpv4AddrPortWidget *>
                                              (ipv4AddrPortWidget));

        }
        else if(attrString == "Protocol")
        {
            QPointer <QObject> protocolWidget =
                    new AttributeProtocolWidget;
            m_objectsToDelete.push_back(protocolWidget);
            m_packetSinkTableWidget->setCellWidget(row, column,
                                                qobject_cast <AttributeProtocolWidget *>
                                                (protocolWidget));

        }

    }
}

void
Applicationswidget::addAppSlot()
{
    //qDebug(QString("addAppSlot"));
    AppManager::AppType_t selectedAppType =
            AppManager::appStringToType(m_supportedAppsComboBox->currentText());
    switch (selectedAppType)
    {
        case AppManager::OnOff:
        {
            addOnOffRow();
            break;
        }
        case AppManager::PacketSink:
        {
            addPacketSinkRow();
            break;
        }
        default:
        {
            NS_FATAL_ERROR("Invalid app selected");
        }
    }

}



void
Applicationswidget::paintEvent(QPaintEvent *event)
{
    m_onOffTableWidget->resizeColumnsToContents();
    m_onOffTableWidget->resizeRowsToContents();
    m_packetSinkTableWidget->resizeColumnsToContents();
    m_packetSinkTableWidget->resizeRowsToContents();
    /*m_onOffTableWidget->setMinimumSize(m_onOffTableWidget->horizontalHeader()->length()+5,
                                       m_onOffTableWidget->horizontalHeader()->height());*/
    QWidget::paintEvent(event);
}

void
Applicationswidget::setNodeId()
{
    DesignerNodeRect * node = (DesignerNodeRect *) m_nodeItem;
    m_nodeId = node->getNodeId();
}

} // namespace netanim
