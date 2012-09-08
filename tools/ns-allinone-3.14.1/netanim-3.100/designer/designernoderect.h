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
#ifndef DESIGNERNODERECT_H
#define DESIGNERNODERECT_H

#include "designerconstants.h"
#include "abstractdesigneritem.h"
#include <QGraphicsRectItem>

namespace netanim {

class DesignerNodeRect : public QGraphicsRectItem,public AbstractDesignerItem
{
public:
    explicit DesignerNodeRect(QPointF center = QPointF(0, 0),int size = DEFAULT_NODE_SIZE);
    void setSize(int size);
    void setCenter(QPointF center);
    uint32_t getNodeId();
    QGraphicsTextItem * getNodeIdText();
    static uint32_t getNodeCount();

private:
    int m_size;
    QPointF m_center;
    uint32_t m_nodeId;
    QGraphicsTextItem * m_nodeIdText;
    ItemType_t getItemType();


};

} // namespace netanim
#endif // DESIGNERNODERECT_H
