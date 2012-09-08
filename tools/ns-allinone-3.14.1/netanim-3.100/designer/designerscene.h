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

#ifndef DESIGNERSCENE_H
#define DESIGNERSCENE_H


#include "animator/animnode.h"
#include "designer/designernoderect.h"
#include "designer/designerp2plinkline.h"
#include "designer/p2plinkmanager.h"
#include "designer/appmanager.h"
#include <QGraphicsScene>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QPainter>

namespace netanim {

class DesignerScene : public QGraphicsScene
{
public:
    typedef std::map <uint64_t, AbstractDesignerItem*> ItemIdItemMap_t;
    typedef enum {Normal, P2PLinkLine, NodeMove} Mode_t;
    explicit DesignerScene(QObject * parent = 0);
    void setCanvasWidth(int value);
    void setCanvasHeight(int value);
    void setCurrentScale(qreal xScale, qreal yScale);
    void addNewNode();
    void setNodeSize(int value);
    void setDesignerSceneMode(Mode_t mode);
    void showLinkManager();
    void showAppManager();
    void play();

private:
    int m_canvasWidth;
    int m_canvasHeight;
    QVector <QGraphicsTextItem*> m_gridCoordinates;
    qreal m_currentXscale;
    qreal m_currentYscale;
    int m_gridLines;
    QVector <DesignerNodeRect *> m_nodes;
    int m_nodeSize;
    Mode_t m_mode;

    // P2PLinkLine
    QGraphicsLineItem * m_activeP2PLinkLine;
    uint64_t m_activeP2PLinkSrcNodeId;


    QGraphicsItem * m_activeMovingNode;
    P2PLinkManager * m_p2pLinkMgr;
    ItemIdItemMap_t m_itemMap;


    AppManager * m_appMgr;

    QRectF getCanvasRect();
    void initGridCoordinates();
    void drawGrid(QPainter * painter);
    void markGridCoordinates();
    void initActiveP2PLinkLine();
    DesignerNodeRect * nodeFromMouseEvent(QGraphicsSceneMouseEvent * event);
    void addItemToMasterList(uint64_t itemId, AbstractDesignerItem * designerItem);

protected:
    void drawBackground(QPainter * painter, const QRectF &rect);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mousePressEventP2PLink(QGraphicsSceneMouseEvent * event);
    void mousePressEventNodeMove(QGraphicsSceneMouseEvent * event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEventP2PLink(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEventNodeMove(QGraphicsSceneMouseEvent * event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEventP2PLink(QGraphicsSceneMouseEvent * event);
    void mouseMoveEventNodeMove(QGraphicsSceneMouseEvent * event);


    void mouseDoubleClickEventNode(QGraphicsSceneMouseEvent * event,AbstractDesignerItem * item);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);



};

} // namespace netanim
#endif // DESIGNERSCENE_H
