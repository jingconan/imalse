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


#include "animnode.h"
#include "debug/xdebug.h"
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QBrush>
#include <QtGui/QPen>

namespace netanim {

AnimNode::AnimNode(uint32_t nodeId,
                   AnimNodeShape shape,
                   qreal width,
                   qreal height,
                   QString description) :
    m_nodeId(nodeId),
    m_shape(shape),
    m_width(width),
    m_height(height),
    m_description(description),
    m_graphicsNodeIdTextItem(0)
{
    switch (shape)
    {
        case CIRCLE:
        m_graphicsItem = new QGraphicsEllipseItem;
            break;
        case RECTANGLE:
        m_graphicsItem = new QGraphicsRectItem;
            break;
        case IMAGE:
            m_graphicsItem = new QGraphicsPixmapItem;
        break;
        default:
            m_graphicsItem = 0;
            // TODO
        break;
    }
    QString nodeDescription;
    if(m_description != "")
        nodeDescription = m_description;
    else
        nodeDescription = QString::number(m_nodeId);
    m_graphicsNodeIdTextItem = new QGraphicsTextItem(nodeDescription);
    m_graphicsNodeIdTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

}


AnimNode::~AnimNode()
{
    if(m_graphicsItem)
        delete m_graphicsItem;
    if(m_graphicsNodeIdTextItem)
        delete m_graphicsNodeIdTextItem;
}

QGraphicsItem *
AnimNode::getGraphicsItem()
{
    return m_graphicsItem;
}



void
AnimNode::setRect(QPointF pos)
{

    //qDebug(pos,"SetRect");
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setColor(Qt::red);
    switch(m_shape)
    {
        case CIRCLE:
        {
            QGraphicsEllipseItem * g = qgraphicsitem_cast<QGraphicsEllipseItem*> (m_graphicsItem);
            g->setRect(pos.x()-m_width/2, pos.y()-m_height/2, m_width, m_height);
            g->setBrush(brush);
            g->setPen(pen);

            break;
        }
        case RECTANGLE:
        {
            QGraphicsRectItem * g = qgraphicsitem_cast<QGraphicsRectItem*> (m_graphicsItem);
            g->setRect(pos.x(), pos.y(), m_width, m_height);
            g->setBrush(brush);
            break;
        }
        case IMAGE:
        break;
    }



}


uint32_t
AnimNode::getNodeId()
{
    return m_nodeId;
}

void
AnimNode::setSize(qreal size)
{
    m_width = m_height = size;
    //qDebug("SetSize");
    setRect(m_graphicsItem->sceneBoundingRect().center());
}



AnimNodeShape
AnimNode::getNodeShape()
{
    return m_shape;
}


void
AnimNode::setPos(QPointF pos)
{
    setRect(pos);
}

QGraphicsTextItem *
AnimNode::getNodeIdTextItem()
{
    return m_graphicsNodeIdTextItem;
}




void
AnimNode::showNodeIdText(bool show)
{

    m_graphicsNodeIdTextItem->setPos(m_graphicsItem->sceneBoundingRect().bottomRight());
    m_graphicsNodeIdTextItem->setVisible(show);

}

} // namespace netanim
