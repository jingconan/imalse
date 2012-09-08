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
#include "designernoderect.h"
#include <QtGui/QBrush>
#include <QtGui/QPen>

namespace netanim {

static uint32_t gNodeId = 0;

DesignerNodeRect::DesignerNodeRect(QPointF center,int size) :
    QGraphicsRectItem(),
    m_size(size),
    m_center(center),
    m_nodeId(gNodeId++)
{
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setColor(Qt::red);
    setSize(m_size);
    setBrush(brush);
    setPen(pen);
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    m_nodeIdText = new QGraphicsTextItem(QString::number(m_nodeId));
    m_nodeIdText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}


void
DesignerNodeRect::setSize(int size)
{
    m_size = size;
    QRectF r = QRectF(0, 0, m_size, m_size);
    r.moveCenter(m_center);
    setRect(r);
}


void
DesignerNodeRect::setCenter(QPointF center)
{
    m_center = center;
    setSize(m_size);

}


AbstractDesignerItem::ItemType_t
DesignerNodeRect::getItemType()
{
    return NodeRect;
}


uint32_t
DesignerNodeRect::getNodeId()
{
    return m_nodeId;
}

QGraphicsTextItem *
DesignerNodeRect::getNodeIdText()
{
    return m_nodeIdText;
}

uint32_t
DesignerNodeRect::getNodeCount()
{
    return gNodeId;
}

} // namespace netanim
