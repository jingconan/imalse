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
#ifndef APPMANAGER_H
#define APPMANAGER_H

#include "designermodule.h"
#include "ns3/core-module.h"
#include "ns3/applications-module.h"

#include <QDialog>
#include <QVector>
#include <QStringList>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>

#include <QTimer>

namespace netanim {

class AppManager : public QDialog
{
Q_OBJECT

public:
    typedef enum {InvalidApp,OnOff,PacketSink} AppType_t;
    typedef QVector <AppType_t> AppTypeVector_t;

    typedef std::vector <ns3::Ptr <ns3::OnOffApplication> > OnOffVector_t;
    typedef std::map <uint64_t, ns3::Ptr <ns3::OnOffApplication> > AppIdOnOffMap_t;


    typedef std::vector <ns3::Ptr <ns3::PacketSink> > PacketSinkVector_t;
    typedef std::map <uint64_t,ns3::Ptr <ns3::PacketSink> > AppIdPacketSinkMap_t;

    typedef std::map <uint64_t,uint32_t> AppIdNodeIdMap_t; //TODO

    AppManager();
    AppTypeVector_t getAvailableApps();
    QStringList getAppAttributeList(AppType_t appType);
    static QString appTypeToString(AppType_t type);
    static AppType_t appStringToType(QString appString);

    void setOnOffVector(uint32_t, OnOffVector_t onOffVec);
    void setPacketSinkVector(uint32_t, PacketSinkVector_t packetSinkVec);
    void showDialog();
    void install();

    AppIdOnOffMap_t getOnOffMap();
    AppIdPacketSinkMap_t getPacketSinkMap();
    uint32_t getNodeIdFromAppId(uint64_t appId);

private:
    AppTypeVector_t m_availableApps;
    AppIdOnOffMap_t m_onOffMap;
    AppIdPacketSinkMap_t m_packetSinkMap;

    QTableWidget * m_appTableWidget;
    QPushButton * m_applyButton;
    QVBoxLayout * m_vLayout;

    QLabel * m_startTimeLabel;
    QLineEdit * m_startTimeLineEdit;
    QLabel * m_stopTimeLabel;
    QLineEdit * m_stopTimeLineEdit;
    AppIdNodeIdMap_t m_appIdNodeIdMap;

    QTableWidget * m_onOffTableWidget;
    QTableWidget * m_packetSinkTableWidget;

    QTimer m_refreshTimer;

    void addAvailableApps();
    void populateTables();
    void addRow();
    void initOnOffTableHeader();
    void initPacketSinkTableHeader();
    void initAppTableHeader();
    void clearOnOffApps(uint32_t nodeId);
    void clearPacketSinks(uint32_t nodeId);

private slots:
    void applySlot();
    void refreshSlot();
};

} // namespace netanim
#endif // APPMANAGER_H
