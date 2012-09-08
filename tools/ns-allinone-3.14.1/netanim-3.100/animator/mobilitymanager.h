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


#ifndef MOBILITYMANAGER_H
#define MOBILITYMANAGER_H

#include <map>
#include <stddef.h>
#include <stdint.h>

#include <QPoint>
#include <QTableWidget>
#include <QProgressBar>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QGraphicsPathItem>

namespace netanim {

struct TimeLocation
{
    double timeStamp;
    QPointF location;
};

class NodeMobilityMgr
{

public:
    typedef std::vector<TimeLocation> TimeLocationVector_t;
    typedef std::map <uint32_t, TimeLocationVector_t::const_iterator> NodeIdTimeLocIteratorMap_t;
    NodeMobilityMgr(void * scene);
    void addNode(double timestamp, uint32_t nodeId, QPointF location);
    QPointF getNodeLocation(uint32_t nodeId);
    bool updateLocations(double currentTime);
    bool isNodeListEmpty();
    uint32_t getEntryCount();
    void populateNodePosTable(QString filter,
                              QString filter2,
                              QString filter3,
                              QString filter4,
                              QTableWidget * tbl,
                              bool showTrajectory,
                              QProgressBar * progressBar);
    void hideAllTrajectoryPaths();
    void systemReset();
    double getFutureTimeDelta();


private:
    typedef std::map <uint32_t, TimeLocationVector_t> NodeIdTimeLocationMap_t;
    NodeIdTimeLocationMap_t m_tsAnimNodes;

    typedef std::map <uint32_t,QPointF> NodeIdLocationMap_t;
    NodeIdLocationMap_t m_nodeLocations;

    typedef std::map <uint32_t, QGraphicsPathItem *> NodeIdPathItemMap_t;
    NodeIdPathItemMap_t m_trajectoryPaths;

    void * m_scene;

    double m_lastTime;

    double m_futureTimeDelta;

    void updateNodeLocationsPerNode(double currentTime, uint32_t nodeId,
                                    const TimeLocationVector_t * tlv);

    void showTrajectory(uint32_t nodeId, TimeLocationVector_t tlv);
    void resetIterator();

};

} // namespace netanim
#endif // MOBILITYMANAGER_H
