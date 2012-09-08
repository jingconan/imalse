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

#ifndef APPLICATIONSWIDGET_H
#define APPLICATIONSWIDGET_H

#include "designermodule.h"

#include "ns3/core-module.h"
#include "ns3/applications-module.h"

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QPushButton>
#include <QPointer>
#include <QLabel>


namespace netanim {

class Applicationswidget : public QWidget
{
    Q_OBJECT
public:
    Applicationswidget(void * nodeItem, void * appManager, QWidget * parent = 0);
    ~Applicationswidget();
private:
    void * m_nodeItem;
    void * m_appManager;
    QComboBox * m_supportedAppsComboBox;
    QVBoxLayout * m_vLayout;
    QHBoxLayout * m_addAppHLayout;
    QPushButton * m_addAppButton;

    QTableWidget * m_onOffTableWidget;
    QTableWidget * m_packetSinkTableWidget;

    QPushButton * m_applyButton;

    uint32_t m_nodeId;

    QLabel * m_onOffLabel;
    QLabel * m_packetSinkLabel;

    void addSupportedApps();
    void addOnOffRow();
    void addPacketSinkRow();
    void initOnOffTable();
    void initPacketSinkTable();


    QVector <QPointer <QObject> > m_objectsToDelete;

    void applyOnOff();
    void applyPacketSink();
    void populateAppTables();
    void populateOnOffTable();
    void populatePacketSinkTable();

    void setNodeId();
    void addOnOffAppToTable(ns3::Ptr <ns3::OnOffApplication>  onOffApp);
    void addPacketSinkToTable(ns3::Ptr <ns3::PacketSink> packetSinkApp);
    void addToObjectsToDelete(QWidget * widget);

protected:
    void paintEvent(QPaintEvent * event);
private slots:
    void addAppSlot();
    void applySlot();

};

} // namespace netanim

#endif // APPLICATIONSWIDGET_H
