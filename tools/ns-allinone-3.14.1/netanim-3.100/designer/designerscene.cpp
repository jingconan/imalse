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
#include "debug/xdebug.h"
#include "designerscene.h"
#include "nodepropertiestab.h"

#include "ns3/internet-module.h"
#include "ns3/network-module.h"

#include <QtGui/QGraphicsSceneMouseEvent>

namespace netanim {

static qreal canvasOffset = DEFAULT_CANVAS_OFFSET;

DesignerScene::DesignerScene(QObject * parent) :
    QGraphicsScene(parent),
    m_canvasWidth(DEFAULT_CANVAS_WIDTH),
    m_canvasHeight(DEFAULT_CANVAS_HEIGHT),
    m_currentXscale(1),
    m_currentYscale(1),
    m_gridLines(5),
    m_nodeSize(DEFAULT_NODE_SIZE),
    m_activeP2PLinkSrcNodeId(0)
{
    initGridCoordinates();
    initActiveP2PLinkLine();
    m_activeMovingNode = 0;
    m_p2pLinkMgr = new P2PLinkManager(&m_itemMap);
    m_appMgr = new AppManager();
}

void
DesignerScene::initActiveP2PLinkLine()
{
    m_activeP2PLinkLine = new QGraphicsLineItem;
    addItem(m_activeP2PLinkLine);
    m_activeP2PLinkLine->setVisible(false);
}

void
DesignerScene::setNodeSize(int value)
{
    m_nodeSize = value;
    for (int i = 0; i < m_nodes.size(); i++)
    {
        m_nodes[i]->setSize(m_nodeSize);
    }
}

void
DesignerScene::setCurrentScale(qreal xScale, qreal yScale)
{
    m_currentXscale = xScale;
    m_currentYscale = yScale;
}

void
DesignerScene::setCanvasHeight(int value)
{
    m_canvasHeight = value;
    update();
}

void
DesignerScene::setCanvasWidth(int value)
{
    m_canvasWidth = value;
    update();
}


void
DesignerScene::initGridCoordinates()
{
    for(int i = 0; i < 9; i++)
    {
        QGraphicsTextItem * item = new QGraphicsTextItem;
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        m_gridCoordinates.push_back(item);
        addItem(item);

    }
}

void
DesignerScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    drawGrid(painter);
    QGraphicsScene::drawBackground(painter,rect);
}

void
DesignerScene::markGridCoordinates()
{
    int i = 0;
    for (qreal x = 0; x <= m_canvasWidth  ; x = x + (m_canvasWidth/2))
    for (qreal y = 0; y <= m_canvasHeight ; y = y + (m_canvasHeight/2))
    {
        QString text = QString::number(qRound(x))
                       + ","
                       + QString::number(qRound(y));
        m_gridCoordinates[i]->setPlainText(text);
        m_gridCoordinates[i]->setPos(QPointF(x,y));
        m_gridCoordinates[i]->resetTransform();
        i++;
    }
}

void
DesignerScene::drawGrid(QPainter *painter)
{
    setSceneRect(getCanvasRect());
    qreal xStep = m_canvasWidth/(m_gridLines-1);
    qreal yStep = m_canvasHeight/(m_gridLines-1);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(100, 100, 155, 125)));
    // draw horizontal grid

    for (qreal y = 0; y <= m_canvasHeight; y += yStep) {
       painter->drawLine(0, y, m_canvasWidth, y);
    }
    // now draw vertical grid
    for (qreal x = 0; x <= m_canvasWidth; x += xStep) {
       painter->drawLine(x, 0, x, m_canvasHeight);
    }
    painter->restore();
    markGridCoordinates();
}



QRectF
DesignerScene::getCanvasRect()
{
    canvasOffset = width()/10;
    QPointF topLeft(-canvasOffset,-canvasOffset);
    QPointF bottomRight(m_canvasHeight+canvasOffset, m_canvasHeight+canvasOffset);

    return QRectF(topLeft, bottomRight);
}

void
DesignerScene::addNewNode()
{

//    DesignerNodeRect * designerNode = new DesignerNodeRect(QPointF(m_canvasWidth/2,m_canvasHeight/2),m_nodeSize);
    DesignerNodeRect * designerNode = new DesignerNodeRect(QPointF(qrand() % m_canvasWidth, qrand() % m_canvasHeight), m_nodeSize);

    addItem(designerNode);
    addItemToMasterList(designerNode->getDesignerItemId(), designerNode);
    QGraphicsTextItem * textItem = designerNode->getNodeIdText();
    QPointF textPos=designerNode->sceneBoundingRect().bottomRight();
    addItem(textItem);
    textItem->setPos(textPos);
    m_nodes.push_back(designerNode);

}


void
DesignerScene::setDesignerSceneMode(Mode_t mode)
{
    m_mode = mode;
}



void
DesignerScene::mouseMoveEventP2PLink(QGraphicsSceneMouseEvent * event)
{
    QLineF line = m_activeP2PLinkLine->line();
    line.setP2(event->scenePos());
    m_activeP2PLinkLine->setLine(line);

}

void
DesignerScene::mouseMoveEventNodeMove(QGraphicsSceneMouseEvent * event)
{
    DesignerNodeRect * nodeItem = nodeFromMouseEvent(event);
    if(!nodeItem)
        return;
    QGraphicsTextItem * textItem = nodeItem->getNodeIdText();
    textItem->setPos(nodeItem->sceneBoundingRect().bottomRight());
}

void
DesignerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch(m_mode)
    {
        case P2PLinkLine:
        {
            mouseMoveEventP2PLink(event);
            break;
        }
        case NodeMove:
        {
            mouseMoveEventNodeMove(event);
        }
        default:
        {

        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void
DesignerScene::mouseReleaseEventP2PLink(QGraphicsSceneMouseEvent * event)
{

    if(m_activeP2PLinkLine->isVisible())
    {
        m_activeP2PLinkLine->setVisible(false);
        DesignerNodeRect * nodeItem = nodeFromMouseEvent(event);
        if(!nodeItem)
            return;
        if(nodeItem->getDesignerItemId() == m_activeP2PLinkSrcNodeId)
            return;
        QRectF nodeRect = nodeItem->sceneBoundingRect();
        DesignerP2PLinkLine * linkLine= new DesignerP2PLinkLine;
        addItem(linkLine);
        addItemToMasterList(linkLine->getDesignerItemId(), linkLine);
        QLineF line = m_activeP2PLinkLine->line();
        line.setP2(nodeRect.center());
        linkLine->setLine(line);

        linkLine->setSrcNodeItemId(m_activeP2PLinkSrcNodeId);
        linkLine->setDstNodeItemId(nodeItem->getDesignerItemId());
        m_p2pLinkMgr->addLinkNodePair(linkLine->getDesignerItemId(),
					m_activeP2PLinkSrcNodeId,
					nodeItem->getDesignerItemId());


    }
    m_activeP2PLinkSrcNodeId = 0;
}


void
DesignerScene::mouseReleaseEventNodeMove(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event);
    m_activeMovingNode->setFlag(QGraphicsItem::ItemIsMovable, false);
    mouseMoveEventNodeMove(event);
}

void
DesignerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug("mouse release");
    switch(m_mode)
    {
        case P2PLinkLine:
        {
            mouseReleaseEventP2PLink(event);
            break;
        }
        case NodeMove:
        {
            mouseReleaseEventNodeMove(event);
            break;
        }
        default:
        {

        }

    }
    QGraphicsScene::mouseReleaseEvent(event);

}

DesignerNodeRect *
DesignerScene::nodeFromMouseEvent(QGraphicsSceneMouseEvent * event)
{
    DesignerNodeRect * nodeItem = 0;
    QList<QGraphicsItem *> itemList = items(event->scenePos());
    foreach(QGraphicsItem * item, itemList)
    {
        AbstractDesignerItem * designerItem = dynamic_cast<AbstractDesignerItem *>(item);
        if(!designerItem)
            continue;
        if(designerItem->getItemType() == AbstractDesignerItem::NodeRect )
        {
            nodeItem = dynamic_cast<DesignerNodeRect*>(designerItem);
            break;
        }

    }
    return nodeItem;

}


void
DesignerScene::mousePressEventP2PLink(QGraphicsSceneMouseEvent * event)
{
    DesignerNodeRect * nodeItem=nodeFromMouseEvent(event);
    if(nodeItem)
    {
        m_activeP2PLinkSrcNodeId = nodeItem->getDesignerItemId();
        QLineF line;
        QPointF nodeCenter = nodeItem->sceneBoundingRect().center();
        line.setP1(nodeCenter);
        line.setP2(nodeCenter);
        m_activeP2PLinkLine->setLine(line);
        m_activeP2PLinkLine->setVisible(true);
    }

}

void
DesignerScene::mousePressEventNodeMove(QGraphicsSceneMouseEvent * event)
{
    DesignerNodeRect * nodeItem=nodeFromMouseEvent(event);
    if(nodeItem)
    {
        nodeItem->setFlag(QGraphicsItem::ItemIsMovable);
        m_activeMovingNode = nodeItem;
    }

}
void
DesignerScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    //qDebug("mouse press");

    switch(m_mode)
    {
        case P2PLinkLine:
        {
            mousePressEventP2PLink(event);
            break;
        }
        case NodeMove:
        {
            mousePressEventNodeMove(event);
            break;
        }
        default:
        {

        }

    }


    QGraphicsScene::mousePressEvent(event);

}

void
DesignerScene::mouseDoubleClickEventNode(QGraphicsSceneMouseEvent * event,AbstractDesignerItem * item)
{
    Q_UNUSED(event);


    DesignerNodeRect * nodeItem = dynamic_cast<DesignerNodeRect *> (item);
    if(!nodeItem)
        return;

    NodePropertiesTab * nodePropertiesTab = new NodePropertiesTab(nodeItem,
                                                                  m_p2pLinkMgr,
                                                                  m_appMgr);
    nodePropertiesTab->exec();

}

void
DesignerScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    AbstractDesignerItem * designerItem = 0;
     QList<QGraphicsItem *> itemList = items(event->scenePos());
     foreach(QGraphicsItem * item, itemList)
     {
         designerItem = dynamic_cast<AbstractDesignerItem *>(item);
         if(!designerItem)
             continue;


     }
     if(!designerItem)
         return;
     switch(designerItem->getItemType())
     {
        case AbstractDesignerItem::NodeRect:
        {
            mouseDoubleClickEventNode(event, designerItem);
            break;
        }
        default:
        {
        }
     }
}


void
DesignerScene::showLinkManager()
{
    m_p2pLinkMgr->showDialog();
}


void
DesignerScene::showAppManager()
{
    m_appMgr->showDialog();
}

void
DesignerScene::addItemToMasterList(uint64_t itemId, AbstractDesignerItem * designerItem)
{
    m_itemMap[itemId] = designerItem;
}


void
DesignerScene::play()
{
    ns3::LogComponentEnable ("PacketSink",ns3::LOG_LEVEL_INFO);

    ns3::NodeContainer nodes;
    nodes.Create (DesignerNodeRect::getNodeCount());
    ns3::InternetStackHelper stack;
    stack.Install (nodes);
    m_p2pLinkMgr->applySimulation();
    m_appMgr->install();
    ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    ns3::Simulator::Run();



}

} // namespace netanim
