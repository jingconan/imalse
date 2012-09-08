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


#include "mobilitymanager.h"
#include "debug/xdebug.h"
#include "animatorscene.h"

#include <QTime>
#include <QApplication>
#include <QPainter>

namespace netanim {

static NodeMobilityMgr::NodeIdTimeLocIteratorMap_t nodeIdTimeLocIterator;

NodeMobilityMgr::NodeMobilityMgr(void * scene):
    m_scene(scene),
    m_lastTime(0),
    m_futureTimeDelta(0)
{

}

double
NodeMobilityMgr::getFutureTimeDelta()
{
   // qDebug(m_futureTimeDelta, "future node");
    return m_futureTimeDelta;
}

void
NodeMobilityMgr::addNode(double timestamp, uint32_t nodeId, QPointF location)
{
    TimeLocation tl;
    tl.location = location;
    tl.timeStamp = timestamp;

    TimeLocationVector_t tlv;
    tlv.push_back(tl);

    if(m_tsAnimNodes.find(nodeId) == m_tsAnimNodes.end() ||
            (timestamp == 0))
    {

       m_tsAnimNodes[nodeId] = tlv;
       if(timestamp == 0)
       {
           m_nodeLocations[nodeId] = location;
       }
       nodeIdTimeLocIterator[nodeId] = m_tsAnimNodes[nodeId].begin();
       return;
    }

    else
    m_tsAnimNodes[nodeId].push_back(tl);
    nodeIdTimeLocIterator[nodeId] = m_tsAnimNodes[nodeId].begin();


}

void
NodeMobilityMgr::systemReset()
{
    m_tsAnimNodes.clear();
    m_nodeLocations.clear();
    AnimatorScene * scene = (AnimatorScene *) m_scene;

    for(NodeIdPathItemMap_t::const_iterator i = m_trajectoryPaths.begin();
        i != m_trajectoryPaths.end();
        ++i)
    {
        QGraphicsPathItem * pathItem = i->second;
        scene->removeItem(pathItem);
        // TODO delete pathItem;
    }
    m_trajectoryPaths.clear();
    nodeIdTimeLocIterator.clear();

}


QPointF
NodeMobilityMgr::getNodeLocation(uint32_t nodeId)
{
    //qDebug(m_nodeLocations[nodeId], "x");
    return m_nodeLocations[nodeId];
}


void
NodeMobilityMgr::updateNodeLocationsPerNode (double currentTime,
                                             uint32_t nodeId,
                                             const TimeLocationVector_t * tlv)
{

    if(tlv->empty())
        return;

    while(nodeIdTimeLocIterator[nodeId] != tlv->end())
    {
        TimeLocation tl = *(nodeIdTimeLocIterator[nodeId]);
        if(tl.timeStamp > currentTime)
        {
            m_futureTimeDelta = tl.timeStamp - currentTime;
            break;
        }
        ++(nodeIdTimeLocIterator[nodeId]);
    }
    if(nodeIdTimeLocIterator[nodeId] == tlv->end())
    {
        m_futureTimeDelta = -1;
        return;
    }
    --(nodeIdTimeLocIterator[nodeId]);
    m_nodeLocations[nodeId] =  (*(nodeIdTimeLocIterator[nodeId])).location;

}

void
NodeMobilityMgr::resetIterator()
{
    for(NodeIdTimeLocationMap_t::const_iterator i = m_tsAnimNodes.begin();
        i != m_tsAnimNodes.end();
        ++i)
    {
        nodeIdTimeLocIterator[i->first] = m_tsAnimNodes[i->first].begin();

    }

}

bool
NodeMobilityMgr::updateLocations(double currentTime)
{
    //qDebug(QString("Mobility Mgr updating node locations"));
    if (m_tsAnimNodes.empty())
    {
        //qDebug(QString("m_tsAnimNodes empty"));
        return false;
    }

    if(m_lastTime > currentTime)
    {
        resetIterator();
    }
    for(NodeIdTimeLocationMap_t::const_iterator i = m_tsAnimNodes.begin();
        i != m_tsAnimNodes.end();
        ++i)
    {
        updateNodeLocationsPerNode(currentTime, i->first, &i->second);

    }
    m_lastTime = currentTime;
    return true;
}


bool
NodeMobilityMgr::isNodeListEmpty()
{
    return m_tsAnimNodes.empty();
}

uint32_t
NodeMobilityMgr::getEntryCount()
{
    return m_tsAnimNodes.size();
}


void
NodeMobilityMgr::showTrajectory(uint32_t nodeId, TimeLocationVector_t tlv)
{
    if(tlv.empty())
        return;
    AnimatorScene * scene = (AnimatorScene *) m_scene;
    if(m_trajectoryPaths.find(nodeId) == m_trajectoryPaths.end())
    {
        QPainterPath path;
        path.moveTo(tlv[0].location);
        for(size_t i = 1; i < tlv.size(); ++i)
        {
            path.lineTo(tlv[i].location);
        }
        m_trajectoryPaths[nodeId] = scene->addPath(path);

    }
    m_trajectoryPaths[nodeId]->show();
    scene->showNode(nodeId);
}

void
NodeMobilityMgr::hideAllTrajectoryPaths()
{

    for(NodeIdPathItemMap_t::const_iterator i = m_trajectoryPaths.begin();
        i != m_trajectoryPaths.end();
        ++i)
    {
        QGraphicsPathItem * pathItem = i->second;
        pathItem->hide();
    }
}

void
NodeMobilityMgr::populateNodePosTable(QString filter1,
                                      QString filter2,
                                      QString filter3,
                                      QString filter4,
                                      QTableWidget *tbl,
                                      bool showTraj,
                                      QProgressBar * progressBar)
{
    QStringList headerList;
    headerList << "Time"
               << "Node Id"
               << "X coord"
               << "Y coord";

    tbl->clearContents();
    tbl->setRowCount(0);
    tbl->setColumnCount(headerList.count());
    tbl->setHorizontalHeaderLabels(headerList);

    int64_t filter1NodeId = -1;
    int64_t filter2NodeId = -1;
    int64_t filter3NodeId = -1;
    int64_t filter4NodeId = -1;

    if(filter1 != "All")
    {
        filter1NodeId = filter1.toUInt();
    }
    if(filter2 != "None")
    {
        filter2NodeId = filter2.toUInt();
    }
    if(filter3 != "None")
    {
        filter3NodeId = filter3.toUInt();
    }
    if(filter4 != "None")
    {
        filter4NodeId = filter4.toUInt();
    }
    QTime t;
    t.start();
    hideAllTrajectoryPaths();

    uint32_t progressBarValue = 0;

    for(NodeIdTimeLocationMap_t::const_iterator i = m_tsAnimNodes.begin();
        i != m_tsAnimNodes.end();
        ++i)
    {
        uint32_t nodeId = i->first;

        if(filter1 != "All")
        {
            if(filter1NodeId == nodeId)
                goto resume;
            if(filter2NodeId == nodeId)
                goto resume;
            if(filter3NodeId == nodeId)
                goto resume;
            if(filter4NodeId == nodeId)
                goto resume;
            continue;
        }
    resume:
        TimeLocationVector_t tlv = i->second;
        if(showTraj)
            showTrajectory(nodeId, tlv);
        for (TimeLocationVector_t::const_iterator j = tlv.begin();
             j != tlv.end();
             ++j)
        {
            TimeLocation tl = *j;
            int row = tbl->rowCount();
            tbl->insertRow(row);
            QTableWidgetItem * wiTime   = new QTableWidgetItem(QString::number(tl.timeStamp));
            QTableWidgetItem * wiNodeId = new QTableWidgetItem(QString::number(nodeId));
            QTableWidgetItem * wiXCoord = new QTableWidgetItem(QString::number(tl.location.x()));
            QTableWidgetItem * wiYCoord = new QTableWidgetItem(QString::number(tl.location.y()));

            tbl->setItem(row, 0, wiTime);
            tbl->setItem(row, 1, wiNodeId);
            tbl->setItem(row, 2, wiXCoord);
            tbl->setItem(row, 3, wiYCoord);

        }
        progressBar->setValue(++progressBarValue);

        if(t.elapsed() > 1000)
        {
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            t.restart();
        }

    }

    tbl->setVisible(true);



}


} // namespace netanim
