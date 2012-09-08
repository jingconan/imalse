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


#ifndef ANIMATORSCENE_H
#define ANIMATORSCENE_H


#include "animnode.h"
#include "animpacket.h"
#include "mobilitymanager.h"
#include "animxmlparser.h"
#include "packetstatisticsdialog.h"
#include "nodepositionstatisticsdialog.h"
#include "nodetrajectorydialog.h"

#include <QGraphicsScene>
#include <QtGui/QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QTimer>
#include <QCheckBox>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QListWidget>
#include <QPushButton>

namespace netanim {

struct WirelessCircleDimensions
{
    QPointF center;
    qreal radius;
};

struct P2PLinkPair
{
    uint32_t nodeA;
    uint32_t nodeB;
};



class PacketStatsProtocolCheckBox : public QCheckBox
{
public:
    PacketStatsProtocolCheckBox(QString protocolName);
    QString getProtocol();
private:
    QString m_protocolName;
};

class AnimatorScene : public QGraphicsScene
{
Q_OBJECT

public:
    explicit AnimatorScene(qreal width = 1024, qreal height = 1024, QObject * parent = 0);
    qreal width();
    qreal height();
    void width(qreal width);
    void height(qreal height);
    bool changeGridStep(bool increase);
    void setGridLinesCount(int nGridLines);
    void showGrid(bool show);
    void addNode(uint32_t nodeId,
                 qreal x,
                 qreal y,
                 AnimNodeShape shape,
                 qreal width,
                 qreal height,
                 QString description);
    void addLink(uint32_t fromId,
                 uint32_t toId);
    void addPacketRx(uint32_t fromId,
                     double fbTx,
                     double lbTx,
                     uint32_t toId,
                     double fbRx,
                     double lbRx,
                     ParsedElementType type,
                     QString metaInfo);
    void setNodeSize(qreal nNodeSize);
    void showNodeId(bool show);
    QVector <QGraphicsSimpleTextItem *> getGridCoordinatesItems();
    void setCurrentScale(qreal xScale,qreal yScale);
    void timeToUpdate(double currentTime, bool fastFwd);
    void test();
    void setMousePositionLabel(QPointF pos);
    void showMousePositionLabel(bool show);
    void showWirelessCircles(bool show);
    bool showPacketStats();
    Packetstatisticsdialog * getPacketStatsDialog();
    bool isPacketListEmpty();
    bool isNodeListEmpty();
    bool showNodePosStats(bool show);
    NodePositionStatisticsDialog * getNodePositionStatsDialog();
    NodeTrajectoryDialog * getNodeTrajectoryDialog();
    void showNode(uint32_t nodeId);
    void blockPacketRendering(bool block);
    void setPktFilterFromId(int64_t id);
    void setPktFilterToId(int64_t id);
    uint32_t getNodeCount();
    void setPacketPrecision(bool precision);
    void systemReset();
    double getFuturePacketDelta();
    double getFutureNodeDelta();

    void updateP2pLinks();
    void softReset();
    void showMeta(bool show);
    void setMetaInfoFontSize(int value);
    void prepareForPlay();
    void setSceneInfoText(QString text, bool show);
    void squareTopo();



private:

    typedef std::map <uint32_t, AnimNode *> AnimNodeMap_t;
    AnimNodeMap_t m_animNodes;
    QVector <QGraphicsSimpleTextItem*> m_gridCoordinates;
    qreal m_width;
    qreal m_height;
    qreal m_gridStep;
    bool  m_showGrid;

    int m_nGridLines;
    qreal m_nodeSize;
    qreal m_currentXscale;
    qreal m_currentYscale;
    bool m_showNodeId;
    NodeMobilityMgr * m_nodeMobilityMgr;

    typedef std::map <double, AnimPacket*> AnimPacketMap_t;
    AnimPacketMap_t m_animPackets;
    typedef std::map <uint32_t, AnimPacket *> PacketToAnimate_t;
    PacketToAnimate_t m_packetsToAnimate;
    QVector <QGraphicsLineItem *> m_pointToPointLinks;
    QLabel * m_mousePositionLabel;
    QGraphicsProxyWidget * m_mousePositionProxyWidget;
    bool m_showWirelessCircles;
    typedef QVector <QGraphicsPathItem *> PathItemVector_t;
    PathItemVector_t m_wirelessCircles;

    QTransform m_textTransform;

    Packetstatisticsdialog m_packetStatsDlg;
    QComboBox m_packetStatsFromIdComboBox;
    QComboBox m_packetStatsToIdComboBox;
    QLineEdit m_packetStatsFbTxLineEdit;
    QTableWidget * m_packetStatsTable;
    QListWidget * m_packetStatsProtocolListWidget;
    QPushButton m_packetStatsProtocolFilterSelectAllButton;
    QPushButton m_packetStatsProtocolFilterDeSelectAllButton;
    QProgressBar m_packetStatsProgressBar;
    QLabel m_packetStatsProgressLabel;

    NodePositionStatisticsDialog m_nodePositionStatsDlg;
    QComboBox m_nodePosStatsIdComboBox;
    QComboBox m_nodePosStatsIdAlt2ComboBox;
    QComboBox m_nodePosStatsIdAlt3ComboBox;
    QComboBox m_nodePosStatsIdAlt4ComboBox;
    QCheckBox m_nodePosStatsTrajectoryCheckBox;

    QTableWidget * m_nodePosStatsTable;
    QProgressBar m_nodePosStatsProgressBar;
    QLabel m_nodePosStatsProgressLabel;

    bool m_showPackets;
    int64_t m_pktFilterFromId;
    int64_t m_pktFilterToId;

    bool m_pktPrecision;

    typedef QVector <QGraphicsLineItem *> LineItemVector_t;
    LineItemVector_t m_gridLines;

    double m_lastTime;
    QPen m_currentPacketPen;

    double m_futurePacketTimeDelta;

    bool m_systemResetInProgress;

    typedef std::vector<P2PLinkPair> P2PLinkPairVector_t;
    P2PLinkPairVector_t m_p2pLinkPairs;

    bool m_softResetInProgress;

    bool m_showMeta;

    int m_metaInfoFontSize;
    QFont m_metaInfoFont;
    //QVector <QGraphicsPathItem *> m_animatedMetaInfo;
    QVector <QGraphicsTextItem *> m_animatedMetaInfo;
    QGraphicsTextItem * m_sceneInfoText;


    QRectF getSceneRect();
    void markGridCoordinates();
    void initGridCoordinates();
    void showGridCoordinates(bool show);
    int isPacketInTimeWindow(const AnimPacket * pkt,double currentTime);
    void packetForwardScan(double currentTime);
    void printAnimPackets();
    AnimPacketMap_t::const_iterator & printAnimPackets(AnimPacketMap_t::const_iterator iter);
    void addToPacketsToAnimate(AnimPacket * packet);
    void purgeOldPackets(double currentTime, bool force = false);
    void printPacketsToAnimate();
    QPointF getNodeLoc(uint32_t nodeId);
    void initPackets();
    void showPackets(double currentTime);
    void showPacket(AnimPacket * packet,double currentTime);
    QPointF getBitPosition(double speed,double currentTime,double bitTime,QLineF linkLine);
    void updateNodeLocations();
    void addToWirelessCircles(WirelessCircleDimensions);
    void resetWirelessCircles();
    void showAllLinkItems(bool show);
    void showAllNodeItems(bool show);

    void addGrid();
    void resetGrid();
    void resetP2pLinks();

    int getProtocolFilterFlags();
    void removeOldMetaInfo();

    QPointF getNodeCenter(uint32_t nodeId);
    QRectF getNodeRect(uint32_t nodeId);

signals:
    
public slots:

protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

protected slots:
    void applyPacketFilterSlot();
    void applyNodePosFilterSlot();

    void packetFilterProtocolSelectAllSlot();
    void packetFilterProtocolDeSelectAllSlot();


};

} // namespace netanim

#endif // ANIMATORSCENE_H
