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

#ifndef AnimatorPlugin_H
#define AnimatorPlugin_H

#include "animatorconstants.h"
#include "animatorscene.h"
#include "animatorview.h"

#include <QWidget>
#include <QtGui/QToolButton>
#include <QtGui/QSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QLCDNumber>
#include <QVBoxLayout>
#include <QSplitter>
#include <QToolBar>
#include <QStatusBar>
#include <QTime>


namespace netanim {

class AnimatorMode: public QWidget
{
    Q_OBJECT

public:
    AnimatorMode();
    QWidget * getCentralWidget();
    QString getTabName();
private:
    //variables
    QVBoxLayout * m_vLayout;
    QLabel * m_gridLinesLabel;
    QLabel * m_nodeSizeLabel;
    QToolButton * m_gridButton;
    QSpinBox * m_gridSizeSpinBox;
    QComboBox * m_nodeSizeComboBox;
    QToolButton * m_testButton;
    QToolButton * m_showNodeIdButton;
    QToolButton * m_playButton;
    QToolButton * m_fileOpenButton;
    QToolButton * m_zoomInButton;
    QToolButton * m_zoomOutButton;
    QToolButton * m_showWirelessCirclesButton;
    QSlider * m_updateRateSlider;
    QLabel * m_updateRateLabel;
    QLCDNumber * m_qLcdNumber;
    bool m_showGrid;
    bool m_showNodeId;
    bool m_playing;
    QTimer * m_updateRateTimer;
    double m_currentTime;
    int m_currentZoomFactor;
    AnimatorScene * m_scene;
    AnimatorView * m_view;
    double m_updateRates[MAX_SLIDER];
    double m_currentUpdateRate;
    QWidget * m_centralWidget;
    bool m_showWiressCircles;
    QDialog * m_parsingXMLDialog;
    double m_parsedMaxSimulationTime;

    QToolBar * m_toolBar;
    QToolButton * m_packetStatsButton;

    QSplitter * m_mainSplitter;

    QVector <QWidget *> m_toolButtonVector;
    QToolButton * m_nodePositionStatsButton;
    QToolButton * m_nodeTrajectoryButton;

    QLabel * m_timelineSpinBoxLabel;
    QSpinBox * m_timelineSpinBox;

    QToolBar * m_verticalToolbar;

    QLabel * m_pktFilterFromLabel;
    QComboBox * m_pktFilterFromComboBox;
    QLabel * m_pktFilterToLabel;
    QComboBox * m_pktFilterToComboBox;

    QToolButton * m_blockPacketsButton;
    QToolBar * m_bottomToolbar;
    QLabel * m_bottomStatusLabel;

    QToolButton * m_precisionButton;
    QToolButton * m_saveButton;

    QToolButton * m_resetButton;
    int m_oldTimelineValue;

    bool m_fastFwd;
    QToolButton * m_fastFwdButton;

    QTime m_appResponsiveTimer;
    bool m_simulationCompleted;

    QToolButton * m_showMetaButton;
    QComboBox * m_packetPersistenceComboBox;
    QLabel * m_packetPersistenceLabel;
    qreal m_packetPersistTime;

    QSpinBox * m_metaFontSpinBox;
    QLabel * m_metaFontLabel;
    QProgressBar * m_parseProgressBar;

    //functions
    bool parseXMLTraceFile(QString traceFileName);
    void setLabelStyleSheet();
    void initUpdateRate();
    void enableAllToolButtons(bool show);
    qreal nodeSizeStringToValue(QString nodeSize);
    void systemReset();

    void updateTimeLineSpinBox(int value);



private slots:
    void gridLinesSlot(int value);
    void nodeSizeSlot(QString value);
    void gridSlot ();
    void traceFileOpenSlot();
    void testSlot();
    void playSlot();
    void showNodeIdSlot();
    void updateRateChangedSlot(int);
    void updateRateTimeoutSlot();
    void zoomInSlot();
    void zoomOutSlot();
    void showWirelessCirclesSlot();
    void packetStatsSlot();
    void nodePositionStatsSlot();
    void timelineSlot(int value);
    void pktFilterFromSlot(QString nodeId);
    void pktFilterToSlot(QString nodeId);
    void showPacketSlot();
    void precisionSlot();
    void saveSlot();
    void resetSlot();
    void fastFwdSlot();
    void showMetaSlot();
    void packetPersistenceSlot(QString value);
    void metaFontSizeSlot(int value);

};


} // namespace netanim

#endif // AnimatorPlugin_H
