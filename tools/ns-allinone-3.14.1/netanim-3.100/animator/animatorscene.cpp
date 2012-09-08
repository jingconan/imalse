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

#include "animator/animatorconstants.h"
#include "animatorscene.h"
#include "debug/xdebug.h"

#include <QtGui/QPainter>
#include <QtGui/QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QtGui/QGraphicsProxyWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QTime>
#include <QFormLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QSvgGenerator>

#include <math.h>


namespace netanim {
static double lastpacketTimestamp = 0; // TODO



void
qDebug(const AnimPacket * pkt)
{

    qDebug(QString("pkt"));
    QString string =
            "fromId:" + QString::number(pkt->m_fromId) + "\n" +
            "toId:"   + QString::number(pkt->m_toId) + "\n" +
            "fbTx:"   + QString::number(pkt->m_fbTx) + "\n"
            "lbTx:"   + QString::number(*pkt->m_lbTx) + "\n" +
            "fbRx:"   + QString::number(*pkt->m_fbRx) + "\n" +
            "lbRx:"   + QString::number(*pkt->m_lbRx) + "\n";
    qDebug(string);
}

void
AnimatorScene::printAnimPackets()
{
    if(m_animPackets.empty())
        return;

    for (AnimPacketMap_t::const_iterator i = m_animPackets.begin();
         i != m_animPackets.end();
         ++i)
    {
        qDebug(i->second);

    }

}


void
AnimatorScene::printPacketsToAnimate()
{

    for(PacketToAnimate_t::const_iterator i = m_packetsToAnimate.begin();
        i != m_packetsToAnimate.end();
        ++i)
    {
        qDebug(i->second);
    }

}
AnimatorScene::AnimPacketMap_t::const_iterator &
AnimatorScene::printAnimPackets(AnimPacketMap_t::const_iterator iter)
{

    qDebug(iter->second);
    return ++iter;
}

AnimatorScene::AnimatorScene(
        qreal width,
        qreal height,
        QObject * parent) :
    QGraphicsScene(parent),
    m_width(width),
    m_height(height),
    m_showGrid(false),
    m_nGridLines(5),
    m_nodeSize(2),
    m_currentXscale(1),
    m_currentYscale(1),
    m_showNodeId(true),
    m_nodeMobilityMgr(0),
    m_showWirelessCircles(false),
    m_showPackets(true),
    m_pktFilterFromId(-1),
    m_pktFilterToId(-1),
    m_pktPrecision(true),
    m_lastTime(0),
    m_futurePacketTimeDelta(0),
    m_systemResetInProgress(true),
    m_softResetInProgress(true),
    m_showMeta(true),
    m_metaInfoFontSize(12)
{
    qreal packetWidth = 4.5;
    m_currentPacketPen.setWidthF(packetWidth / m_currentXscale);
    m_currentPacketPen.setColor(Qt::blue);
    m_mousePositionLabel = new QLabel;
    m_mousePositionProxyWidget = addWidget(m_mousePositionLabel,Qt::ToolTip);
    m_mousePositionProxyWidget->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    m_nodeMobilityMgr = new NodeMobilityMgr(this);
    m_textTransform.scale(1,1);
    m_metaInfoFont.setPointSize(m_metaInfoFontSize);

    m_sceneInfoText = new QGraphicsTextItem;
    m_sceneInfoText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    addItem(m_sceneInfoText);

}


void
AnimatorScene::setSceneInfoText(QString text, bool show)
{
    m_sceneInfoText->setPlainText(text);
    m_sceneInfoText->setVisible(show);
    QPointF pos = QPointF((m_width - text.length())/4, sceneRect().center().y());
    m_sceneInfoText->setPos(pos);
}


void
AnimatorScene::setMetaInfoFontSize(int value)
{
    m_metaInfoFontSize = value;
    m_metaInfoFont.setPointSize(m_metaInfoFontSize);
    AnimPacket::setFontSize(m_metaInfoFontSize);

}

void
AnimatorScene::showMeta(bool show)
{
    m_showMeta = show;
}

void
AnimatorScene::setPktFilterFromId(int64_t id)
{
    m_pktFilterFromId = id;
}


void
AnimatorScene::setPktFilterToId(int64_t id)
{
    m_pktFilterToId = id;
}

void
AnimatorScene::showGrid(bool show)
{
    m_showGrid = show;
    resetGrid();
    if(m_showGrid)
        addGrid();
    showGridCoordinates(m_showGrid);
    update();
}

void PrintRect (QRectF rect)
{
    QString rectS = "BL:" + QString::number(rect.bottomLeft().x()) + "," +
                                            QString::number(rect.bottomLeft().y()) ;
    qDebug(rectS);
    rectS         = "TR:" + QString::number(rect.topRight().x()) + "," +
                                            QString::number(rect.topRight().y());
    qDebug(rectS);
    rectS         = "TL:" + QString::number(rect.topLeft().x()) + "," +
                                            QString::number(rect.topLeft().y());
    qDebug(rectS);
}


void
AnimatorScene::setMousePositionLabel(QPointF pos)
{

    QString string = "    (" + QString::number(qRound(pos.x())) + "," + QString::number(qRound(pos.y())) + ")";

    m_mousePositionLabel->setText(string);
    m_mousePositionLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_mousePositionProxyWidget->setPos(pos.x(), pos.y());
    m_mousePositionLabel->adjustSize();

}

void
AnimatorScene::setCurrentScale(qreal xScale, qreal yScale)
{
//    qDebug(QString("Current Scale:")+ QString::number(m_currentXscale)+QString(" ")+QString::number(m_currentYscale));
//    qDebug(QString("Set Current Scale:")+ QString::number(xScale)+QString(" ")+QString::number(yScale));
    m_currentXscale = xScale;
    m_currentYscale = yScale;
    m_textTransform.scale(1/m_currentXscale, 1/m_currentYscale);

}

QRectF
AnimatorScene::getSceneRect()
{
    qreal sceneOffsetX = width()/10;
    qreal sceneOffsetY = height()/10;
    QPointF topLeft(-2 * sceneOffsetX, -sceneOffsetY);
    QPointF bottomRight(m_width+  sceneOffsetX, m_height+ 2 * sceneOffsetY);

    return QRectF(topLeft, bottomRight);
}


void
AnimatorScene::drawForeground(QPainter * painter, const QRectF &rect)
{
    //qDebug ("Drawing foreground");

    setSceneRect(getSceneRect());
    QGraphicsScene::drawForeground(painter, rect);
}


void
AnimatorScene::drawBackground(QPainter * painter, const QRectF &rect)
{

    setSceneRect(getSceneRect());
    QGraphicsScene::drawBackground(painter, rect);
}



void
AnimatorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
      QPointF scenePos = event->scenePos();
 //   QString s = "Mouse:" + QString::number(event->scenePos().x()) + "," + QString::number(event->scenePos().y());
 //   qDebug(s.toAscii().data());
    setMousePositionLabel(scenePos);
    if((scenePos.x() < 0) ||
       (scenePos.y() < 0) ||
       (scenePos.x() > width()) ||
       (scenePos.y() > height()))
    {
        showMousePositionLabel(false);
    }
    else
    {
        showMousePositionLabel(true);
    }
    QGraphicsScene::mouseMoveEvent(event);

}


qreal
AnimatorScene::width()
{
    return m_width;
}

void
AnimatorScene::width(qreal width)
{
    m_width = width;
    initGridCoordinates();

}

qreal
AnimatorScene::height()
{
    return m_height;
}

void
AnimatorScene::height(qreal height)
{
    m_height = height;
    initGridCoordinates();

}

void
AnimatorScene::setGridLinesCount(int nGridLines)
{
    m_nGridLines = nGridLines;
    resetGrid();
    if(m_showGrid)
        addGrid();
    initGridCoordinates();
    update();
}

QPointF
AnimatorScene::getNodeCenter(uint32_t nodeId)
{
    AnimNode * node = m_animNodes[nodeId];
    return node->getGraphicsItem()->boundingRect().center();
}


void
AnimatorScene::updateNodeLocations()
{
    for (AnimNodeMap_t::const_iterator i = m_animNodes.begin();
         i != m_animNodes.end();
         ++i)
    {
        AnimNode * aNode = i->second;
        aNode->setPos(m_nodeMobilityMgr->getNodeLocation(i->first));
        //qDebug(m_nodeMobilityMgr->getNodeLocation(i->first), QString::number(i->first));
        if(m_showNodeId)
            aNode->getNodeIdTextItem()->setPos(aNode->getGraphicsItem()->boundingRect().center());
    }

}

void
AnimatorScene::setNodeSize(qreal nNodeSize)
{
    //qDebug(QString("Current Scale:")+ QString::number(m_currentXscale)+QString(" ")+QString::number(m_currentYscale));
    m_nodeSize = nNodeSize/m_currentXscale * 10;
    qreal packetWidth = 4.5;
    m_currentPacketPen.setWidthF(packetWidth / m_currentXscale);
    for (AnimNodeMap_t::const_iterator i = m_animNodes.begin();
         i != m_animNodes.end();
         ++i)
    {
        AnimNode * aNode = i->second;
        aNode->setSize(m_nodeSize);
    }
    showNodeId(m_showNodeId);
    update();

}

bool
AnimatorScene::changeGridStep(bool increase)
{
    if(increase)
        m_gridStep *= 2;
    else
        m_gridStep /= 2;
    update();
    if((m_gridStep >= 100) && (increase))
        return false;
    if((m_gridStep <  1) && (!increase))
        return false;
    initGridCoordinates();
    return true;
}


void
AnimatorScene::initGridCoordinates()
{

    for (int i = 0; i < m_gridCoordinates.size(); ++i)
    {
        QGraphicsSimpleTextItem * item = m_gridCoordinates[i];
        removeItem(item);
        delete item;
    }
    m_gridCoordinates.clear();
    for(int i = 0; i < 9; i++)
    {
        QGraphicsSimpleTextItem * item = new QGraphicsSimpleTextItem;
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        m_gridCoordinates.push_back(item);
        addItem(item);

    }
    markGridCoordinates();

}

QVector <QGraphicsSimpleTextItem *>
AnimatorScene::getGridCoordinatesItems()
{
    return m_gridCoordinates;
}


void
AnimatorScene::showGridCoordinates(bool show)
{
    for (int i = 0; i < m_gridCoordinates.size(); i++)
    {
        m_gridCoordinates[i]->setVisible(show);

    }
}


void
AnimatorScene::markGridCoordinates()
{
    int i = 0;
    for (qreal x = 0; x <= m_width  ; x = x + (m_width/2))
    for (qreal y = 0; y <= m_height ; y = y + (m_height/2))
    {
        QString text = QString::number(qRound(x))
                       + ","
                       + QString::number(qRound(y));
        m_gridCoordinates[i]->setText(text);
        m_gridCoordinates[i]->setPos(QPointF(x, y));
        m_gridCoordinates[i]->setVisible(m_showGrid);
        i++;
    }

}

void
AnimatorScene::addGrid()
{
    qreal xStep = m_width/(m_nGridLines-1);
    qreal yStep = m_height/(m_nGridLines-1);
    QPen pen(QColor(100, 100, 155, 125));

    // draw horizontal grid

    for (qreal y = 0; y <= m_height; y += yStep) {
       m_gridLines.push_back(addLine(0, y, m_width, y, pen));
    }
    // now draw vertical grid
    for (qreal x = 0; x <= m_width; x += xStep) {
       m_gridLines.push_back(addLine(x, 0, x, m_height, pen));
    }
}

void
AnimatorScene::resetGrid()
{
    for(LineItemVector_t::const_iterator i = m_gridLines.begin();
        i != m_gridLines.end();
        ++i)
    {

        removeItem(*i);
        delete (*i);
    }
    m_gridLines.clear();
}


void
AnimatorScene::test()
{
   /* static AnimPacketMap_t::const_iterator iter = m_animPackets.begin();
    if (!m_animPackets.empty())
    iter = printAnimPackets(iter);*/

   // printPacketsToAnimate();
    qDebug(m_packetsToAnimate.size(),"PacketsToAnimate");
}

bool
AnimatorScene::isNodeListEmpty()
{
    return m_nodeMobilityMgr->isNodeListEmpty();
}


void
AnimatorScene::showNodeId(bool show)
{

    m_showNodeId = show;

    for (AnimNodeMap_t::const_iterator i = m_animNodes.begin();
     i != m_animNodes.end();
     ++i)
    {
        AnimNode * aNode = i->second;
        aNode->showNodeIdText(show);

    }
    if(m_nodePositionStatsDlg.isVisible())
    {
        showAllNodeItems(false);
        applyNodePosFilterSlot();
    }

}

void
AnimatorScene::addPacketRx(uint32_t fromId, double fbTx, 
			double lbTx, uint32_t toId, 
			double fbRx, double lbRx,
            ParsedElementType type,
            QString metaInfo)
{
    lastpacketTimestamp = lbRx;// TODO
    if(fromId == toId)
        return;
    AnimPacket * packet = new AnimPacket(fromId, toId, 
					fbTx, lbTx, 
					fbRx, lbRx, 
					(type == XML_WPACKET_RX)?1:0);
    m_animPackets[packet->getAnimId()] = packet;
    Q_UNUSED(metaInfo)

   if ((metaInfo != "null") && (m_showMeta))
      packet->parseMeta(metaInfo.toAscii().data());
}

void
AnimatorScene::addLink(uint32_t fromId, uint32_t toId)
{
    P2PLinkPair p = {fromId, toId};
    m_p2pLinkPairs.push_back(p);
    //qDebug(QString ("Adding link:") + QString::number(fromId) + QString::number(toId));
}

void
AnimatorScene::addNode(uint32_t nodeId,
                       qreal x,
                       qreal y,
                       AnimNodeShape shape,
                       qreal width,
                       qreal height,
                       QString description)
{
    AnimNode * aNode = 0;

    //qDebug(QString("Adding node id:") + QString::number(nodeId));
    if(m_animNodes.find(nodeId) == m_animNodes.end())
    {
        aNode = new AnimNode(nodeId,
                             shape,
                             width,
                             height,
                             description);
        addItem(aNode->getGraphicsItem());
        addItem(aNode->getNodeIdTextItem());
        aNode->showNodeIdText(m_showNodeId);
        m_animNodes[nodeId] = aNode;

    }
    else
    {
        aNode = m_animNodes[nodeId];
    }

    QPointF pos(x, y);
    aNode->setPos(pos);
    m_nodeMobilityMgr->addNode(lastpacketTimestamp, nodeId, pos);

}


double
AnimatorScene::getFuturePacketDelta()
{
    return m_futurePacketTimeDelta;
}

double
AnimatorScene::getFutureNodeDelta()
{
    return m_nodeMobilityMgr->getFutureTimeDelta();
}

int
AnimatorScene::isPacketInTimeWindow(const AnimPacket *pkt, double currentTime)
{
    // 1 In the past
    // 2 In the future
    if((pkt->m_lbRx) && (currentTime > *pkt->m_lbRx))
    {
        //qDebug("Packet in past");
        return 1;
    }
    if (currentTime < pkt->m_fbTx)
    {
        //qDebug("Packet in future");
        m_futurePacketTimeDelta = pkt->m_fbTx - currentTime;
        return 2;
    }
    return 0;

}


void
AnimatorScene::timeToUpdate(double currentTime, bool fastFwd)
{
    //qDebug("Timetoupdate");
    bool nodesUpdated = m_nodeMobilityMgr->updateLocations(currentTime);
    if(nodesUpdated)
    {
        //qDebug(QString("***Node updated"));
        updateNodeLocations();
    }
    if(fastFwd)
    removeOldMetaInfo();
    if (m_animPackets.empty())
        return;
    packetForwardScan(currentTime);
    showPackets(currentTime);
    m_lastTime = currentTime;

}

void
AnimatorScene::purgeOldPackets(double currentTime, bool force)
{
    QVector <uint64_t> purgeList;
    for (PacketToAnimate_t::const_iterator i = m_packetsToAnimate.begin();
         i != m_packetsToAnimate.end();
         ++i)
    {
        AnimPacket * packet = i->second;
        if((packet->m_isWpacket && (currentTime > packet->m_fbTx)) || force)
        {
            purgeList.push_back(packet->getAnimId());
            removeItem(packet->m_graphicsItem);
            if(packet->m_graphicsTextItem->scene())
                removeItem(packet->m_graphicsTextItem);
            continue;
        }
        if(packet->m_isWpacket)
            continue;
        if ((*packet->m_lbRx < currentTime) || force)
        {
            purgeList.push_back(packet->getAnimId());
            removeItem(packet->m_graphicsItem);
            if(packet->m_graphicsTextItem->scene())
                removeItem(packet->m_graphicsTextItem);
        }

    }
    //qDebug(purgeList.size(),"size of purge list");
    for (QVector <uint64_t>::const_iterator i = purgeList.begin();
         i != purgeList.end();
         ++i)
    {
        m_packetsToAnimate.erase(*i);
    }

}

void
AnimatorScene::showMousePositionLabel(bool show)
{
    m_mousePositionProxyWidget->setVisible(show);
}

QPointF
AnimatorScene::getNodeLoc(uint32_t nodeId)
{
    return m_nodeMobilityMgr->getNodeLocation(nodeId);
}


QPointF
AnimatorScene::getBitPosition(double speed, double currentTime, double bitTime, QLineF linkLine)
{
    double timeDelta = currentTime - bitTime;
    double distanceTraveled = timeDelta * speed;
    linkLine.setLength(distanceTraveled);
    return linkLine.p2();

}

void
AnimatorScene::prepareForPlay()
{
    m_textTransform.scale(1/m_currentXscale, 1/m_currentYscale);
    AnimPacket::setFontSize(m_metaInfoFontSize);
}

void
AnimatorScene::showPacket(AnimPacket * packet,double currentTime)
{
    if(!m_showPackets)
        return;
    if(!packet->m_selected)
        return;
    if(packet->m_fromId == packet->m_toId)
        return;

    QPointF fromNodeLoc = getNodeCenter(packet->m_fromId);
    QPointF toNodeLoc = getNodeCenter(packet->m_toId);


    QLineF linkLine(fromNodeLoc, toNodeLoc);
    double fbRx = 0;
    if(packet->m_fbRx)
        fbRx = *packet->m_fbRx;
    double propagationDelay = fbRx - packet->m_fbTx;

    double speed = linkLine.length()/propagationDelay;

    QLineF packetLine(linkLine);

    if((!packet->m_isWpacket) && (m_pktPrecision))
     {
        if(*packet->m_lbTx > currentTime)
        {
            packetLine.setP1(fromNodeLoc);
        }
        else
        {
            packetLine.setP1(
                        getBitPosition(speed, currentTime,
                                        *packet->m_lbTx, linkLine));
        }

        if(*packet->m_fbRx < currentTime)
        {
            packetLine.setP2(toNodeLoc);

        }
        else
        {
            packetLine.setP2(
                        getBitPosition(speed, currentTime,
					packet->m_fbTx, linkLine));
        }
    }
    else
    {
        if(m_showWirelessCircles && packet->m_isWpacket)
        {
            WirelessCircleDimensions dimensions = { fromNodeLoc, packetLine.length() };
            addToWirelessCircles(dimensions);
        }
    }

    qreal angle = packetLine.angle();
    packet->m_graphicsItem->setLine(packetLine);
    packet->m_graphicsItem->setPen(m_currentPacketPen);

    if (!m_showMeta)
        return;
    angle = 360-angle;

    if(packet->getShortMeta() == "")
        return;

    if(fromNodeLoc.y() < toNodeLoc.y() &&
            fromNodeLoc.x() > toNodeLoc.x() )
    {
        angle += 180;
        packet->setTextTransform("<--"+ packet->getShortMeta() , angle);
        QLineF l(getNodeRect(packet->m_toId).topLeft(), getNodeRect(packet->m_fromId).topLeft());

        l.setLength(l.length()/4);
        packet->m_graphicsTextItem->setPos(l.p2());
        return;
    }

    if(fromNodeLoc.y() < toNodeLoc.y() &&
            fromNodeLoc.x() == toNodeLoc.x())
    {
        packet->setTextTransform("<--"+ packet->getShortMeta(), angle);
        QLineF l(getNodeRect(packet->m_fromId).topLeft(), getNodeRect(packet->m_toId).topLeft());
        l.setLength(l.length()/4);
        packet->m_graphicsTextItem->setPos(l.p2());
        return;

    }

    if(fromNodeLoc.y() >= toNodeLoc.y() &&
            fromNodeLoc.x() >= toNodeLoc.x())
    {
        angle += 180;
        packet->setTextTransform("<--"+ packet->getShortMeta(), angle);
        QLineF l(getNodeRect(packet->m_toId).topRight(), getNodeRect(packet->m_fromId).topRight());
        l.setLength(l.length()/4);
        packet->m_graphicsTextItem->setPos(l.p2());

        return;
    }

    if(fromNodeLoc.y() < toNodeLoc.y() &&
        fromNodeLoc.x() < toNodeLoc.x())
    {
        packet->setTextTransform(packet->getShortMeta() + "-->" , angle);
        QLineF l(getNodeRect(packet->m_fromId).bottomLeft(), getNodeRect(packet->m_toId).bottomLeft());
        l.setLength(l.length()/4);
        packet->m_graphicsTextItem->setPos(l.p2());
        return;

    }

    if(fromNodeLoc.y() > toNodeLoc.y() &&
        fromNodeLoc.x() > toNodeLoc.x())
    {
        packet->setTextTransform(packet->getShortMeta() + "-->" , angle);
        QLineF l(getNodeRect(packet->m_fromId).bottomLeft(), getNodeRect(packet->m_toId).bottomLeft());
        l.setLength(l.length()/4);
        packet->m_graphicsTextItem->setPos(l.p2());
        return;
    }
    packet->setTextTransform(packet->getShortMeta() + "-->" , angle);
    QLineF l(getNodeRect(packet->m_fromId).topLeft(), getNodeRect(packet->m_toId).topLeft());
    l.setLength(l.length()/4);
    packet->m_graphicsTextItem->setPos(l.p2());

}


void
AnimatorScene::resetWirelessCircles()
{
    for(PathItemVector_t::const_iterator i = m_wirelessCircles.begin();
        i != m_wirelessCircles.end();
        ++i)
    {
        removeItem(*i);
        delete (*i);
    }
    m_wirelessCircles.clear();
}

void
AnimatorScene::addToWirelessCircles(WirelessCircleDimensions dimensions)
{
    QPainterPath path;
    path.addEllipse(dimensions.center, dimensions.radius/4, dimensions.radius/4);
    path.addEllipse(dimensions.center, dimensions.radius/2, dimensions.radius/2);
    path.addEllipse(dimensions.center, dimensions.radius, dimensions.radius);
    QGraphicsPathItem * pathItem = addPath(path);
    m_wirelessCircles.push_back(pathItem);
}

void
AnimatorScene::showPackets(double currentTime)
{

    purgeOldPackets(currentTime);
    if(m_showWirelessCircles)
        resetWirelessCircles();
    for (PacketToAnimate_t::const_iterator i = m_packetsToAnimate.begin();
         i != m_packetsToAnimate.end();
         ++i)
    {
        showPacket(i->second,currentTime);
    }
}


void
AnimatorScene::packetForwardScan(double currentTime)
{

//    qDebug("Packetforwardscan");
    static AnimPacketMap_t::const_iterator staticIterator ;
    if (m_systemResetInProgress || m_softResetInProgress)
    {
       staticIterator = m_animPackets.begin();
       m_systemResetInProgress = false;
       m_softResetInProgress = false;

    }


    if(m_lastTime > currentTime)
    {
        staticIterator = m_animPackets.begin();
        purgeOldPackets(currentTime, true);
    }

    m_futurePacketTimeDelta = 0;

    AnimPacketMap_t::const_iterator i = staticIterator;
    if(i == m_animPackets.end())
    {
        m_futurePacketTimeDelta = -1;
        return;
    }
    for (;
         i != m_animPackets.end();
         ++i)
    {
        int retCode = isPacketInTimeWindow(i->second, currentTime);
        if (retCode == 0)
        {
            addToPacketsToAnimate(i->second);
        }
        else if (retCode == 1)
        {
           // qDebug("Packet in Past");
            staticIterator = i;
        }
        else
        {
            break;
        }

    }

    return;

}


void
AnimatorScene::addToPacketsToAnimate(AnimPacket * packet)
{
    //qDebug("Adding packets to animate");
    uint64_t animId = packet->getAnimId();
    if(m_packetsToAnimate.find(animId) == m_packetsToAnimate.end())
    {
        addItem(packet->m_graphicsItem);
        addItem(packet->m_graphicsTextItem);
        packet->m_graphicsTextItem->setVisible(m_showMeta);
        m_animatedMetaInfo.push_back(packet->m_graphicsTextItem);
    }
    else
    {
        //qDebug("Id present. Skipping");
        return;
    }

    m_packetsToAnimate[animId] = packet;
}


void
AnimatorScene::removeOldMetaInfo()
{

    for(int i = 0; i < m_animatedMetaInfo.size(); ++i)
    {
        if(m_animatedMetaInfo[i]->scene())
        removeItem(m_animatedMetaInfo[i]);
    }
}


void
AnimatorScene::showWirelessCircles(bool show)
{
    m_showWirelessCircles = show;
    if(!show)
        resetWirelessCircles();
}

uint32_t
AnimatorScene::getNodeCount()
{
    if(m_animNodes.empty())
        return 0;
    return (m_animNodes.end())->first;

}

int
AnimatorScene::getProtocolFilterFlags()
{
    int flags = AnimPacket::None;
    int rowCount = m_packetStatsProtocolListWidget->count();
    for (int row = 0; row < rowCount; ++row)
    {
      PacketStatsProtocolCheckBox * pCheckBox =
              dynamic_cast<PacketStatsProtocolCheckBox *>
              (m_packetStatsProtocolListWidget->itemWidget(m_packetStatsProtocolListWidget->item(row)));
      if(pCheckBox && pCheckBox->isChecked())
      {
       QString protocol = pCheckBox->getProtocol();
       if(protocol == "All Packets")
           flags  |= AnimPacket::All;
       if(protocol == "Arp")
           flags  |= AnimPacket::Arp;
       if(protocol == "Tcp")
           flags  |= AnimPacket::Tcp;
       if(protocol == "Udp")
           flags  |= AnimPacket::Udp;
       if(protocol == "Icmpv4")
           flags  |= AnimPacket::Icmp;
       if(protocol == "Ipv4")
           flags  |= AnimPacket::Ipv4;
       if(protocol == "Ppp")
           flags  |= AnimPacket::Ppp;
       if(protocol == "Ethernet")
           flags  |= AnimPacket::Ethernet;
       if(protocol == "Wifi")
           flags  |= AnimPacket::Wifi;
       if(protocol == "Dsdv")
           flags  |= AnimPacket::Dsdv;
       if(protocol == "Aodv")
           flags  |= AnimPacket::Aodv;
       if(protocol == "Olsr")
           flags  |= AnimPacket::Olsr;
      }

    }
    return flags;

}

void
AnimatorScene::applyPacketFilterSlot()
{
    QStringList headerList;
    headerList << "Tx Time"
               << "From Node Id"
               << "To Node Id"
               << " Meta Info";
    m_packetStatsTable->clearContents();
    m_packetStatsTable->setRowCount(0);
    m_packetStatsTable->setColumnCount(headerList.count());
    m_packetStatsTable->setHorizontalHeaderLabels(headerList);

    uint32_t progressBarValue = 0;
    m_packetStatsProgressBar.setVisible(true);
    m_packetStatsProgressLabel.setVisible(true);
    QTime t;
    t.start();
    m_packetStatsDlg.setAllowClose(false);
    for(AnimPacketMap_t::const_iterator i = m_animPackets.begin();
        i != m_animPackets.end();
        ++i)
    {
        AnimPacket * pkt = i->second;

        if(pkt->m_fbTx < m_packetStatsFbTxLineEdit.text().toDouble())
        {
            pkt->m_selected = false;
            continue;

        }
        if((m_packetStatsFromIdComboBox.currentText() != "All") &&
                (pkt->m_fromId != m_packetStatsFromIdComboBox.currentText().toUInt()))
        {
            pkt->m_selected = false;
            continue;
        }
        if((m_packetStatsToIdComboBox.currentText() != "All") &&
                (pkt->m_toId != m_packetStatsToIdComboBox.currentText().toUInt()))
        {
            pkt->m_selected = false;
            continue;
        }
        QString metaInfo = "";
        if((metaInfo = QString(pkt->getMeta(getProtocolFilterFlags()).c_str())) == "")
        {
            pkt->m_selected = false;
            continue;
        }
        pkt->m_selected = true;

        int row = m_packetStatsTable->rowCount();
        m_packetStatsTable->insertRow(row);
        QTableWidgetItem * wiFbTx = new QTableWidgetItem(QString::number(pkt->m_fbTx));
        QTableWidgetItem * wiFromId = new QTableWidgetItem(QString::number(pkt->m_fromId));
        QTableWidgetItem * wiToId = new QTableWidgetItem(QString::number(pkt->m_toId));
        QTableWidgetItem * wiMeta = new QTableWidgetItem(metaInfo);
        m_packetStatsTable->setItem(row, 0, wiFbTx);
        m_packetStatsTable->setItem(row, 1, wiFromId);
        m_packetStatsTable->setItem(row, 2, wiToId);
        m_packetStatsTable->setItem(row, 3, wiMeta);
        m_packetStatsProgressBar.setValue(progressBarValue++);
        if(t.elapsed() > 1000)
        {
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            t.restart();
        }
    }
    m_packetStatsTable->setVisible(true);
    m_packetStatsDlg.setAllowClose(true);
    m_packetStatsProgressBar.setVisible(false);
    m_packetStatsProgressLabel.setVisible(false);
    m_packetStatsTable->adjustSize();
    m_packetStatsTable->resizeColumnsToContents();
}


bool
AnimatorScene::isPacketListEmpty()
{
    return m_animPackets.empty();

}

void
AnimatorScene::applyNodePosFilterSlot()
{
    m_nodePosStatsProgressBar.setVisible(true);
    m_nodePosStatsProgressLabel.setVisible(true);
    if(m_nodePosStatsTrajectoryCheckBox.isChecked())
    {
        showAllLinkItems(false);
        showAllNodeItems(false);
    }
    else
    {
        showAllLinkItems(true);
        showAllNodeItems(true);
    }
    m_nodeMobilityMgr->populateNodePosTable(m_nodePosStatsIdComboBox.currentText(),
                                            m_nodePosStatsIdAlt2ComboBox.currentText(),
                                            m_nodePosStatsIdAlt3ComboBox.currentText(),
                                            m_nodePosStatsIdAlt4ComboBox.currentText(),
                                            m_nodePosStatsTable,
                                            m_nodePosStatsTrajectoryCheckBox.isChecked(),
                                            &m_nodePosStatsProgressBar);



    m_nodePosStatsTable->resizeColumnsToContents();
    m_nodePositionStatsDlg.setMinimumWidth(m_nodePosStatsTable->sizeHint().width()+5);
    m_nodePosStatsProgressBar.setVisible(false);
    m_nodePosStatsProgressLabel.setVisible(false);
}

bool
AnimatorScene::showNodePosStats(bool show)
{
    m_nodePositionStatsDlg.setVisible(show);
    if(!show)
    {
        showAllLinkItems(true);
        showAllNodeItems(true);
        m_nodeMobilityMgr->hideAllTrajectoryPaths();
        return false;
    }
    if(m_nodeMobilityMgr->isNodeListEmpty())
    {
        QMessageBox msg;
        msg.setText("No Nodes have been parsed. Did you load the xml trace file?");
        msg.exec();
        return false;
    }
    m_nodePositionStatsDlg.setWindowTitle("Node Position statistics");

    QVBoxLayout vLayout;
    vLayout.setSpacing(5);
    QFormLayout formLayout;
    QLabel entryCountLabel(QString::number(m_nodeMobilityMgr->getEntryCount()));
    formLayout.addRow("Entry count", &entryCountLabel);

    QStringList nodeList;
    QStringList nodeListAlt;
    nodeList << "All";
    nodeListAlt << "None";
    for(uint32_t i = 0; i < getNodeCount(); ++i)
    {
        nodeList << QString::number(i);
        nodeListAlt << QString::number(i);
    }
    m_nodePosStatsIdComboBox.clear();
    m_nodePosStatsIdAlt2ComboBox.clear();
    m_nodePosStatsIdAlt3ComboBox.clear();
    m_nodePosStatsIdAlt4ComboBox.clear();

    m_nodePosStatsIdComboBox.addItems(nodeList);
    m_nodePosStatsIdAlt2ComboBox.addItems(nodeListAlt);
    m_nodePosStatsIdAlt3ComboBox.addItems(nodeListAlt);
    m_nodePosStatsIdAlt4ComboBox.addItems(nodeListAlt);


    QPushButton applyButton("Apply filter");
    applyButton.setToolTip("Apply filter");
    applyButton.adjustSize();
    //applyButton.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(&applyButton, SIGNAL(clicked()), this, SLOT(applyNodePosFilterSlot()));

    m_nodePosStatsProgressLabel.setText("Parsing progress:");
    m_nodePosStatsProgressBar.setRange(0, entryCountLabel.text().toUInt());
    m_nodePosStatsProgressBar.setVisible(false);
    m_nodePosStatsProgressLabel.setVisible(false);


    m_nodePosStatsTable = new QTableWidget;
    m_nodePosStatsTable->setVisible(false);

    formLayout.addRow("Node Id", &m_nodePosStatsIdComboBox);
    formLayout.addRow("Add Node Id", &m_nodePosStatsIdAlt2ComboBox);
    formLayout.addRow("Add Node Id", &m_nodePosStatsIdAlt3ComboBox);
    formLayout.addRow("Add Node Id", &m_nodePosStatsIdAlt4ComboBox);

    formLayout.setFormAlignment(Qt::AlignLeft);
    formLayout.setSpacing(5);
    formLayout.setLabelAlignment(Qt::AlignLeft);

    formLayout.addWidget(&applyButton);
    formLayout.addRow("Show Trajectory", &m_nodePosStatsTrajectoryCheckBox);
    formLayout.addWidget(&m_nodePosStatsProgressLabel);
    formLayout.addWidget(&m_nodePosStatsProgressBar);

    vLayout.addLayout(&formLayout);
    vLayout.addWidget(m_nodePosStatsTable);
    m_nodePositionStatsDlg.setLayout(&vLayout);
    m_nodePositionStatsDlg.setMinimumWidth(applyButton.sizeHint().width() * 2);
    m_nodePositionStatsDlg.setVisible(true);
    m_nodePositionStatsDlg.exec();
    delete m_nodePosStatsTable;
    m_nodePosStatsProgressBar.reset();
    return true;
}


PacketStatsProtocolCheckBox::PacketStatsProtocolCheckBox(QString protocolName):
   QCheckBox(protocolName), m_protocolName(protocolName)
{

}


QString
PacketStatsProtocolCheckBox::getProtocol()
{
    return m_protocolName;
}

bool
AnimatorScene::showPacketStats()
{
    if(m_animPackets.empty())
    {
        QMessageBox msg;
        msg.setText("No packets have been parsed. Did you load the xml trace file?");
        msg.exec();
        return false;
    }
    m_packetStatsDlg.setWindowTitle("Packet statistics");

    QHBoxLayout hLayout;
    QVBoxLayout vLayout;
    vLayout.setSpacing(5);
    QFormLayout formLayout;
    QLabel packetCountLabel(QString::number(m_animPackets.size()));
    formLayout.addRow("Packet count", &packetCountLabel);



    QStringList nodeList;
    nodeList << "All";
    for(uint32_t i = 0; i < getNodeCount(); ++i)
    {
        nodeList << QString::number(i);
    }
    m_packetStatsFromIdComboBox.clear();
    m_packetStatsToIdComboBox.clear();
    m_packetStatsFromIdComboBox.addItems(nodeList);
    m_packetStatsToIdComboBox.addItems(nodeList);

    QLabel fbTxLabel("Transmission Time >=");
    fbTxLabel.setToolTip("First bit transmission time greater than and equal to");
    m_packetStatsFbTxLineEdit.setText("0");


    QPushButton applyButton("Apply filter");
    applyButton.setToolTip("Apply filter");
    applyButton.adjustSize();
    applyButton.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    applyButton.setGeometry(0, 0, 5, 5);
    connect(&applyButton, SIGNAL(clicked()), this, SLOT(applyPacketFilterSlot()));


    m_packetStatsProgressLabel.setText("Parsing progress:");
    m_packetStatsProgressBar.setRange(0, m_animPackets.size());
    m_packetStatsProgressBar.setVisible(false);
    m_packetStatsProgressLabel.setVisible(false);


    m_packetStatsTable = new QTableWidget;
    m_packetStatsTable->setVisible(false);

    m_packetStatsProtocolListWidget = new QListWidget(&m_packetStatsDlg);
    m_packetStatsProtocolFilterSelectAllButton.setText("Select All");
    m_packetStatsProtocolFilterDeSelectAllButton.setText("DeSelect All");
    connect(&m_packetStatsProtocolFilterSelectAllButton, SIGNAL(clicked()), this, SLOT(packetFilterProtocolSelectAllSlot()));
    connect(&m_packetStatsProtocolFilterDeSelectAllButton, SIGNAL(clicked()), this, SLOT(packetFilterProtocolDeSelectAllSlot()));


    QListWidgetItem * all = new QListWidgetItem(m_packetStatsProtocolListWidget);
    PacketStatsProtocolCheckBox * allPacketsCB = new PacketStatsProtocolCheckBox("All Packets");
    m_packetStatsProtocolListWidget->setItemWidget(all, allPacketsCB);
    allPacketsCB->setChecked(true);
    m_packetStatsProtocolListWidget->addItem(all);
    QListWidgetItem * ethernet = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(ethernet, new PacketStatsProtocolCheckBox("Ethernet"));
    m_packetStatsProtocolListWidget->addItem(ethernet);
    QListWidgetItem * ppp = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(ppp, new PacketStatsProtocolCheckBox("Ppp"));
    m_packetStatsProtocolListWidget->addItem(ppp);
    QListWidgetItem * wifi = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(wifi, new PacketStatsProtocolCheckBox("Wifi"));
    m_packetStatsProtocolListWidget->addItem(wifi);
    QListWidgetItem * arp = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(arp, new PacketStatsProtocolCheckBox("Arp"));
    m_packetStatsProtocolListWidget->addItem(arp);
    QListWidgetItem * ipv4 = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(ipv4, new PacketStatsProtocolCheckBox("Ipv4"));
    m_packetStatsProtocolListWidget->addItem(ipv4);
    QListWidgetItem * icmpv4 = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(icmpv4, new PacketStatsProtocolCheckBox("Icmpv4"));
    m_packetStatsProtocolListWidget->addItem(icmpv4);
    QListWidgetItem * udp = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(udp, new PacketStatsProtocolCheckBox("Udp"));
    m_packetStatsProtocolListWidget->addItem(udp);
    QListWidgetItem * tcp = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(tcp, new PacketStatsProtocolCheckBox("Tcp"));
    m_packetStatsProtocolListWidget->addItem(tcp);
    QListWidgetItem * aodv = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(aodv, new PacketStatsProtocolCheckBox("Aodv"));
    m_packetStatsProtocolListWidget->addItem(aodv);
    QListWidgetItem * olsr = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(olsr, new PacketStatsProtocolCheckBox("Olsr"));
    m_packetStatsProtocolListWidget->addItem(olsr);
    QListWidgetItem * dsdv = new QListWidgetItem(m_packetStatsProtocolListWidget);
    m_packetStatsProtocolListWidget->setItemWidget(dsdv, new PacketStatsProtocolCheckBox("Dsdv"));
    m_packetStatsProtocolListWidget->addItem(dsdv);


    formLayout.addRow("From Node Id", &m_packetStatsFromIdComboBox);
    formLayout.addRow("To Node Id", &m_packetStatsToIdComboBox);
    formLayout.setFormAlignment(Qt::AlignLeft);
    formLayout.setSpacing(5);
    formLayout.setLabelAlignment(Qt::AlignLeft);

    formLayout.addRow("Transmission time >=", &m_packetStatsFbTxLineEdit);
    formLayout.setFormAlignment(Qt::AlignLeft);
    formLayout.setSpacing(5);



    formLayout.addWidget(&applyButton);
    formLayout.addWidget(&m_packetStatsProgressLabel);
    formLayout.addWidget(&m_packetStatsProgressBar);
    formLayout.addWidget(&m_packetStatsProtocolFilterSelectAllButton);
    formLayout.addWidget(&m_packetStatsProtocolFilterDeSelectAllButton);


    vLayout.addLayout(&formLayout);
    vLayout.addWidget(m_packetStatsProtocolListWidget);

    hLayout.addLayout(&vLayout);
    hLayout.addWidget(m_packetStatsTable);
    hLayout.setStretchFactor(m_packetStatsTable, 5);

    m_packetStatsDlg.setLayout(&hLayout);
    m_packetStatsDlg.setVisible(true);
    m_packetStatsDlg.exec();
    delete m_packetStatsTable;
    delete m_packetStatsProtocolListWidget;
    m_packetStatsProgressBar.reset();
    return true;

}

void
AnimatorScene::packetFilterProtocolSelectAllSlot()
{
    int rowCount = m_packetStatsProtocolListWidget->count();
    for (int row = 0; row < rowCount; ++row)
    {
      PacketStatsProtocolCheckBox * pCheckBox =
              dynamic_cast<PacketStatsProtocolCheckBox *>
              (m_packetStatsProtocolListWidget->itemWidget(m_packetStatsProtocolListWidget->item(row)));
      pCheckBox->setChecked(true);
    }
}

void
AnimatorScene::packetFilterProtocolDeSelectAllSlot()
{
    int rowCount = m_packetStatsProtocolListWidget->count();
    for (int row = 0; row < rowCount; ++row)
    {
      PacketStatsProtocolCheckBox * pCheckBox =
              dynamic_cast<PacketStatsProtocolCheckBox *>
              (m_packetStatsProtocolListWidget->itemWidget(m_packetStatsProtocolListWidget->item(row)));
      pCheckBox->setChecked(false);
    }

}


NodePositionStatisticsDialog *
AnimatorScene::getNodePositionStatsDialog()
{
    return &m_nodePositionStatsDlg;
}

Packetstatisticsdialog *
AnimatorScene::getPacketStatsDialog()
{
    return &m_packetStatsDlg;
}

void
AnimatorScene::showAllLinkItems(bool show)
{
    for(int i = 0; i < m_pointToPointLinks.size(); ++i)
    {
        m_pointToPointLinks[i]->setVisible(show);
    }

}


void
AnimatorScene::showAllNodeItems(bool show)
{

    for(uint32_t i = 0; i < m_animNodes.size(); ++i)
    {
        m_animNodes[i]->getGraphicsItem()->setVisible(show);
        if(m_showNodeId)
        m_animNodes[i]->getNodeIdTextItem()->setVisible(show);

    }

}

void
AnimatorScene::showNode(uint32_t nodeId)
{
    m_animNodes[nodeId]->getGraphicsItem()->setVisible(true);
    if(m_showNodeId)
    m_animNodes[nodeId]->getNodeIdTextItem()->setVisible(true);
}

void
AnimatorScene::blockPacketRendering(bool block)
{
    m_showPackets = !block;
}


void
AnimatorScene::setPacketPrecision(bool precision)
{
    m_pktPrecision = precision;
}

void
AnimatorScene::systemReset()
{
   // qDebug(QString("Scene system reset"));
    m_systemResetInProgress = true;
    // remove animnodes
    removeOldMetaInfo();
    m_animatedMetaInfo.clear();
    for (AnimNodeMap_t::const_iterator i = m_animNodes.begin();
         i != m_animNodes.end();
         ++i)
    {
        removeItem(i->second->getGraphicsItem());
        delete (i->second);
    }
    m_animNodes.clear();


    // remove animPackets
    purgeOldPackets(0, true);
    for (AnimPacketMap_t::const_iterator i = m_animPackets.begin();
         i != m_animPackets.end();
         ++i)
    {
        delete (i->second);
    }
    m_animPackets.clear();
  //  qDebug(QString("All animPackets cleared"));



    resetWirelessCircles();
    m_nodeMobilityMgr->systemReset();
    delete m_nodeMobilityMgr;
    m_nodeMobilityMgr = new NodeMobilityMgr(this);
    lastpacketTimestamp = 0;
    m_currentXscale = m_currentYscale = 1;
    AnimPacket::systemReset();
    m_lastTime = 0;
    resetP2pLinks();
    m_futurePacketTimeDelta = 0;
    invalidate();

}

void
AnimatorScene::resetP2pLinks()
{
    // remove links
    for(QVector <QGraphicsLineItem *>::const_iterator i = m_pointToPointLinks.begin();
        i != m_pointToPointLinks.end();
        ++i)
    {
        removeItem(*i);
        delete (*i);
    }
    m_pointToPointLinks.clear();
    for(QVector <QGraphicsLineItem *>::const_iterator i = m_pointToPointLinks.begin();
        i != m_pointToPointLinks.end();
        ++i)
    {
        removeItem(*i);
        delete (*i);
    }
    m_p2pLinkPairs.clear();

}

void
AnimatorScene::softReset()
{
    m_softResetInProgress = true;
    purgeOldPackets(10000, true);
    lastpacketTimestamp = 0;
    m_futurePacketTimeDelta = 0;

}

void
AnimatorScene::updateP2pLinks()
{

    for (P2PLinkPairVector_t::const_iterator i = m_p2pLinkPairs.begin();
         i != m_p2pLinkPairs.end();
         ++i)
    {
        P2PLinkPair p = *i;
        QPointF fromNodeLoc = getNodeLoc(p.nodeA);
        QPointF toNodeLoc = getNodeLoc(p.nodeB);
        QLineF line(fromNodeLoc, toNodeLoc);
        m_animNodes[p.nodeA]->setPos(line.p1());
        m_animNodes[p.nodeB]->setPos(line.p2());
        QGraphicsLineItem * item = new QGraphicsLineItem(line);

        m_pointToPointLinks.push_back(item);
        addItem(item);

    }

}

QRectF
AnimatorScene::getNodeRect(uint32_t nodeId)
{
    AnimNode * node = m_animNodes[nodeId];
    return node->getGraphicsItem()->boundingRect();
}

void
AnimatorScene::squareTopo()
{
    qreal maxValue = qMax(m_width, m_height);
    m_width = maxValue;
    m_height = maxValue;
}

} // namespace netanim
