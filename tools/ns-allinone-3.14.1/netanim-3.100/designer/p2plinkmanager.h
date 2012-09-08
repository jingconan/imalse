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
#ifndef P2PLINKMANAGER_H
#define P2PLINKMANAGER_H

#include "designerp2plinkline.h"

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <map>

namespace netanim {

class P2PLinkManager : public QDialog
{
Q_OBJECT

public:
    typedef std::pair <uint64_t, uint32_t> LinkItemIdNodeIdPair_t;
    typedef QVector<LinkItemIdNodeIdPair_t> LinkItemIdNodeIdPairVector_t;
    P2PLinkManager(void * itemMap);
    void addLinkNodePair(uint64_t linkId, uint64_t nodeA, uint64_t nodeB);
    LinkItemIdNodeIdPairVector_t getPeerNodeId(uint64_t myNodeItemId);
    void setLinkProperties(uint64_t linkItemId, QString dataRate, QString delay);
    DesignerP2PLinkLine * getP2PLinkLike(uint64_t itemId);
    void showDialog();
    void applySimulation();
    void applyLinkItem(DesignerP2PLinkLine * linkItem);
private:
    typedef std::pair <uint64_t,uint64_t> NodeItemIdPair_t;
    typedef std::map <uint64_t,NodeItemIdPair_t> LinkItemIdSrcDstNodeItemIdPair_t;

    LinkItemIdSrcDstNodeItemIdPair_t m_linkNodes;
    void * m_itemMap;

    QTableWidget * m_p2pTableWidget;
    QPushButton * m_applyButton;
    QVBoxLayout * m_vLayout;


    uint32_t getNodeIdfromItemId (uint64_t itemId);
    void initP2PTableHeader();
    void populateTables();
    void addRow(DesignerP2PLinkLine * linkItem);
    QString getColumnName(int column);

private slots:
    void applySlot();


};

} // namespace netanim

#endif // P2PLINKMANAGER_H
