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

#include "animatormode.h"
#include "animatorscene.h"
#include "animatorview.h"
#include "animxmlparser.h"
#include "debug/xdebug.h"

#include <QFile>
#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QMacStyle>
#include <QGraphicsTextItem>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>


namespace netanim {

 AnimatorMode::AnimatorMode():
     m_showGrid(true),
     m_showNodeId(false),
     m_playing(false),
     m_updateRateTimer(0),
     m_currentTime(0),
     m_currentZoomFactor(1),
     m_showWiressCircles(false),
     m_parsingXMLDialog(0),
     m_parsedMaxSimulationTime(5000),
     m_oldTimelineValue(0),
     m_fastFwd(true),
     m_simulationCompleted(false),
     m_packetPersistTime(500)
 {

        m_scene = new AnimatorScene;
        m_vLayout = new QVBoxLayout;
        m_toolBar = new QToolBar;
        QSize iconSize(20, 20);
        m_toolBar->setIconSize(iconSize);

        m_gridLinesLabel = new QLabel("Lines");
        m_nodeSizeLabel = new QLabel("Node Size");
        m_packetPersistenceLabel = new QLabel("Persist");

        m_fileOpenButton = new QToolButton;
        m_fileOpenButton->setToolTip("Open XML trace file");
        m_fileOpenButton->setIcon(QIcon(":/animator_resource/animator_fileopen.svg"));
        m_fileOpenButton->setGeometry(0, 0, 5, 20);
        connect(m_fileOpenButton,SIGNAL(clicked()), this, SLOT(traceFileOpenSlot()));


        m_zoomInButton = new QToolButton;
        m_zoomInButton->setToolTip("Zoom in");
        m_zoomInButton->setIcon(QIcon(":/animator_resource/animator_zoomin.svg"));
        connect(m_zoomInButton, SIGNAL(clicked()), this, SLOT(zoomInSlot()));
        m_zoomInButton->setGeometry(0, 0, 5, 20);


        m_zoomOutButton = new QToolButton;
        m_zoomOutButton->setToolTip("Zoom out");
        m_zoomOutButton->setIcon(QIcon(":/animator_resource/animator_zoomout.svg"));
        connect(m_zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOutSlot()));
        m_zoomOutButton->setGeometry(0, 0, 5, 20);


        m_gridButton = new QToolButton;
        if(!m_showGrid)
        m_gridButton->setToolTip("Turn ON Grid");
        m_gridButton->setIcon(QIcon(":/animator_resource/animator_grid.svg"));
        m_gridButton->setGeometry(0, 0, 5, 20);
        m_gridButton->setCheckable(true);
        connect(m_gridButton, SIGNAL(clicked()), this, SLOT(gridSlot()));


        m_gridSizeSpinBox = new QSpinBox;
        m_gridSizeSpinBox->setToolTip("Set the number of grid lines");
        m_gridSizeSpinBox->setRange(3, 21);
        m_gridSizeSpinBox->setSingleStep(2);
        connect(m_gridSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(gridLinesSlot(int)));
        m_gridSizeSpinBox->setValue(5);
        m_gridSizeSpinBox->setGeometry(0, 0, 5, 20);



        m_nodeSizeComboBox = new QComboBox;
        m_nodeSizeComboBox->setToolTip("Node Size");
        QStringList nodeSizes;
        nodeSizes << "20%"
                  << "40%"
                  << "50%"
                  << "60%"
                  << "80%"
                  << "100%"
                  << "200%"
                  << "300%"
                  << "400%"
                  << "500%"
                  << "600%"
                  << "900%"
                  << "1000%"
                  << "2000%";
        m_nodeSizeComboBox->addItems(nodeSizes);
        connect(m_nodeSizeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(nodeSizeSlot(QString)));
        m_nodeSizeComboBox->setCurrentIndex(7);
        m_nodeSizeComboBox->setGeometry(0, 0, 0, 10);


        m_packetPersistenceComboBox = new QComboBox;
        m_packetPersistenceComboBox->setToolTip("Duration for which the packet should be seen on screen if using fast-forward");
        QStringList persistTimes;
        persistTimes << "10ms"
                     << "50ms"
                     << "100ms"
                     << "250ms"
                     << "500ms"
                     << "1s";
        m_packetPersistenceComboBox->addItems(persistTimes);
        connect(m_packetPersistenceComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(packetPersistenceSlot(QString)));
        m_packetPersistenceComboBox->setCurrentIndex(4);
        m_packetPersistenceComboBox->setGeometry(0, 0, 0, 10);


        m_testButton = new QToolButton;
        m_testButton->setText("T");
        m_testButton->setGeometry(0, 0, 5, 20);
        connect(m_testButton, SIGNAL(clicked()), this, SLOT(testSlot()));




        m_showNodeIdButton = new QToolButton;
        m_showNodeIdButton->setIcon(QIcon(":/animator_resource/animator_nodeid.svg"));
        m_showNodeIdButton->setGeometry(0, 0, 5, 20);
        m_showNodeIdButton->setToolTip("Show Node Id");
        m_showNodeIdButton->setCheckable(true);
        connect(m_showNodeIdButton, SIGNAL(clicked()), this, SLOT(showNodeIdSlot()));
        m_showNodeIdButton->setGeometry(0, 0, 10, 20);


        m_playButton = new QToolButton;
        m_playButton->setIcon(QIcon(":/animator_resource/animator_play.svg"));
        m_playButton->setGeometry(0, 0, 5, 20);
        m_playButton->setToolTip("Play Animation");
        connect(m_playButton, SIGNAL(clicked()), this, SLOT(playSlot()));


        m_updateRateSlider = new QSlider(Qt::Horizontal);
        m_updateRateSlider->setToolTip("Animation update interval");
        m_updateRateSlider->setRange (0, MAX_SLIDER);
        m_updateRateSlider->setGeometry(0, 0, 5, 20);
        m_updateRateSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(m_updateRateSlider, SIGNAL(valueChanged(int)), this, SLOT(updateRateChangedSlot(int)));


        m_updateRateLabel = new QLabel;
        m_updateRateLabel->setToolTip("Current update interval");
        m_updateRateSlider->setGeometry(0, 0, 40, 20);
        m_updateRateSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


        m_timelineSpinBoxLabel = new QLabel("Sim time");
        m_timelineSpinBoxLabel->setToolTip("Set current time");
        m_timelineSpinBoxLabel->setGeometry(0, 0, 5, 20);
        m_timelineSpinBox = new QSpinBox;
        m_timelineSpinBox->setToolTip("Set current time");
        m_timelineSpinBox->setRange(0, 100);
        m_timelineSpinBox->setValue(0);
        m_timelineSpinBox->setSingleStep(1);
        m_timelineSpinBox->setGeometry(0, 0, 5, 20);
        connect(m_timelineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(timelineSlot(int)));


        m_metaFontLabel = new QLabel("Meta Font");
        m_metaFontSpinBox = new QSpinBox;
        m_metaFontSpinBox->setToolTip("Set the font size for packet meta-data");
        m_metaFontSpinBox->setRange(0, 100);
        m_metaFontSpinBox->setValue(0);
        m_metaFontSpinBox->setSingleStep(1);
        m_metaFontSpinBox->setGeometry(0, 0, 5, 20);
        connect(m_metaFontSpinBox, SIGNAL(valueChanged(int)), this, SLOT(metaFontSizeSlot(int)));



        m_qLcdNumber = new QLCDNumber;
        m_qLcdNumber->setAutoFillBackground(true);
        m_qLcdNumber->setToolTip("Current simulation time");
        m_qLcdNumber->setStyleSheet("QLCDNumber {background:black; color: black;}");
        m_qLcdNumber->setGeometry(0, 0, 10, 10);



        m_showWirelessCirclesButton = new QToolButton;
        m_showWirelessCirclesButton->setIcon(QIcon(":/animator_resource/animator_wirelesscircles.svg"));
        m_showWirelessCirclesButton->setGeometry(0,0,10,20);
        m_showWirelessCirclesButton->setToolTip("Toggle Show Wireless Circles Animation");
        m_showWirelessCirclesButton->setCheckable(true);
        connect(m_showWirelessCirclesButton, SIGNAL(clicked()), this, SLOT(showWirelessCirclesSlot()));


        m_packetStatsButton = new QToolButton;
        m_packetStatsButton->setIcon(QIcon(":/animator_resource/animator_packetstats.svg"));
        m_packetStatsButton->setGeometry(0,0,5,20);
        m_packetStatsButton->setToolTip("Packet filter and statistics");
        connect(m_packetStatsButton, SIGNAL(clicked()), this, SLOT(packetStatsSlot()));
        m_packetStatsButton->setCheckable(true);



        m_nodePositionStatsButton = new QToolButton;
        m_nodePositionStatsButton->setIcon(QIcon(":/animator_resource/animator_trajectory.svg"));
        m_nodePositionStatsButton->setGeometry(0,0,5,20);
        m_nodePositionStatsButton->setToolTip("Node Position statistics");
        connect(m_nodePositionStatsButton, SIGNAL(clicked()), this, SLOT(nodePositionStatsSlot()));
        m_nodePositionStatsButton->setCheckable(true);


        m_pktFilterFromLabel = new QLabel("From:");
        m_pktFilterFromLabel->setToolTip("Show packets with from Id");
        m_pktFilterFromLabel->setGeometry(0, 0, 5, 20);
        m_pktFilterFromComboBox = new QComboBox;
        m_pktFilterFromComboBox->setToolTip("Show packets with from Id");
        m_pktFilterFromComboBox->addItem("All");
        connect(m_pktFilterFromComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(pktFilterFromSlot(QString)));

        m_pktFilterToLabel = new QLabel("To:");
        m_pktFilterToLabel->setToolTip("Show packets with to Id");
        m_pktFilterToLabel->setGeometry(0, 0, 5, 20);
        m_pktFilterToComboBox = new QComboBox;
        m_pktFilterToComboBox->setToolTip("Show packets with to Id");
        m_pktFilterToComboBox->addItem("All");
        connect(m_pktFilterToComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(pktFilterToSlot(QString)));


        m_blockPacketsButton = new QToolButton;
        m_blockPacketsButton->setIcon(QIcon(":/animator_resource/animator_showpackets.svg"));
        m_blockPacketsButton->setGeometry(0,0,5,20);
        m_blockPacketsButton->setToolTip("Show packets");
        connect(m_blockPacketsButton, SIGNAL(clicked()), this, SLOT(showPacketSlot()));
        m_blockPacketsButton->setCheckable(true);

        m_precisionButton = new QToolButton;
        m_precisionButton->setText("P");
        m_precisionButton->setToolTip("Toggle packet animation precision. \
                                      Precision is needed to produce finer graphics in point-to-point links");
        m_precisionButton->setGeometry(0,0,5,20);
        m_precisionButton->setCheckable(true);
        connect(m_precisionButton, SIGNAL(clicked()), this, SLOT(precisionSlot()));


        m_saveButton = new QToolButton;
        m_saveButton->setIcon(QIcon(":/animator_resource/animator_save.svg"));
        m_saveButton->setGeometry(0,0,5,20);
        m_saveButton->setToolTip("Save canvas as an image in SVG format");
        connect(m_saveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));


        m_resetButton = new QToolButton;
        m_resetButton->setText("R");
        m_resetButton->setGeometry(0,0,5,20);
        m_resetButton->setToolTip("Reset Simulation to start");
        connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));


        m_fastFwdButton = new QToolButton;
        m_fastFwdButton->setIcon(QIcon(":/animator_resource/animator_fastfwd.svg"));
        m_fastFwdButton->setCheckable(true);
        m_fastFwdButton->setGeometry(0,0,5,20);
        m_fastFwdButton->setToolTip("Fast forward. Use simulation time and bypass update rate timer");
        connect(m_fastFwdButton, SIGNAL(clicked()), this, SLOT(fastFwdSlot()));

        m_showMetaButton = new QToolButton;
        m_showMetaButton->setText("M");
        m_showMetaButton->setCheckable(true);
        m_showMetaButton->setGeometry(0,0,5,20);
        m_showMetaButton->setToolTip("Show Packet meta data");
        connect(m_showMetaButton, SIGNAL(clicked()), this, SLOT(showMetaSlot()));

        m_toolBar->addWidget(m_fileOpenButton);
        m_toolBar->addSeparator();
        m_toolBar->addWidget(m_playButton);
        m_toolBar->addWidget(m_updateRateSlider);
        m_toolBar->addWidget(m_updateRateLabel);
        m_toolBar->addSeparator();
        m_toolBar->addWidget(m_timelineSpinBoxLabel);
        m_toolBar->addWidget(m_timelineSpinBox);
        m_toolBar->addWidget(m_qLcdNumber);

        m_toolBar->addSeparator();
        m_toolBar->addWidget(m_packetPersistenceLabel);
        m_toolBar->addWidget(m_packetPersistenceComboBox);
        m_toolBar->addWidget(m_gridButton);
        m_toolBar->addWidget(m_gridLinesLabel);
        m_toolBar->addWidget(m_gridSizeSpinBox);
        m_toolBar->addSeparator();
        m_toolBar->addWidget(m_nodeSizeLabel);
        m_toolBar->addWidget(m_nodeSizeComboBox);
        m_toolBar->addSeparator();
        m_toolBar->addWidget(m_metaFontLabel);
        m_toolBar->addWidget(m_metaFontSpinBox);

        //tool->addWidget(m_testButton);

        m_toolButtonVector.push_back(m_playButton);
        m_toolButtonVector.push_back(m_updateRateSlider);
        m_toolButtonVector.push_back(m_updateRateLabel);
        m_toolButtonVector.push_back(m_timelineSpinBoxLabel);
        m_toolButtonVector.push_back(m_timelineSpinBox);
        m_toolButtonVector.push_back(m_gridButton);
        m_toolButtonVector.push_back(m_gridLinesLabel);
        m_toolButtonVector.push_back(m_gridSizeSpinBox);
        m_toolButtonVector.push_back(m_zoomInButton);
        m_toolButtonVector.push_back(m_zoomOutButton);
        m_toolButtonVector.push_back(m_nodeSizeLabel);
        m_toolButtonVector.push_back(m_nodeSizeComboBox);
        m_toolButtonVector.push_back(m_showNodeIdButton);
        m_toolButtonVector.push_back(m_qLcdNumber);
        m_toolButtonVector.push_back(m_showWirelessCirclesButton);
        m_toolButtonVector.push_back(m_packetStatsButton);
        m_toolButtonVector.push_back(m_nodePositionStatsButton);
        m_toolButtonVector.push_back(m_pktFilterFromLabel);
        m_toolButtonVector.push_back(m_pktFilterFromComboBox);
        m_toolButtonVector.push_back(m_pktFilterToLabel);
        m_toolButtonVector.push_back(m_pktFilterToComboBox);
        m_toolButtonVector.push_back(m_precisionButton);
        m_toolButtonVector.push_back(m_saveButton);
        m_toolButtonVector.push_back(m_blockPacketsButton);
        m_toolButtonVector.push_back(m_resetButton);
        m_toolButtonVector.push_back(m_fastFwdButton);
        m_toolButtonVector.push_back(m_packetPersistenceComboBox);
        m_toolButtonVector.push_back(m_packetPersistenceLabel);
        m_toolButtonVector.push_back(m_metaFontLabel);
        m_toolButtonVector.push_back(m_metaFontSpinBox);

        m_view = new AnimatorView(m_scene);

        m_verticalToolbar = new QToolBar;
        m_verticalToolbar->setOrientation(Qt::Vertical);
        m_verticalToolbar->addWidget(m_fastFwdButton);
        m_verticalToolbar->addWidget(m_zoomInButton);
        m_verticalToolbar->addWidget(m_zoomOutButton);
        m_verticalToolbar->addSeparator();
        m_verticalToolbar->addWidget(m_showNodeIdButton);
        m_verticalToolbar->addSeparator();
        m_verticalToolbar->addWidget(m_showWirelessCirclesButton);
        m_verticalToolbar->addSeparator();
        m_verticalToolbar->addWidget(m_packetStatsButton);
        m_verticalToolbar->addWidget(m_nodePositionStatsButton);
        m_verticalToolbar->addWidget(m_blockPacketsButton);
        m_verticalToolbar->addWidget(m_precisionButton);
        m_verticalToolbar->addWidget(m_saveButton);
        m_verticalToolbar->addWidget(m_resetButton);
        m_verticalToolbar->addWidget(m_showMetaButton);
        m_verticalToolbar->setIconSize(iconSize);


        m_bottomToolbar = new QToolBar;
        m_bottomStatusLabel = new QLabel;
        m_parseProgressBar = new QProgressBar;
        m_bottomToolbar->addWidget(m_bottomStatusLabel);
        m_bottomToolbar->addWidget(m_parseProgressBar);
        m_parseProgressBar->setVisible(false);

        m_mainSplitter = new QSplitter;
        m_mainSplitter->addWidget(m_verticalToolbar);
        m_mainSplitter->addWidget(m_view);
        m_mainSplitter->addWidget(m_scene->getPacketStatsDialog());
        m_mainSplitter->addWidget(m_scene->getNodePositionStatsDialog());

        m_vLayout->setSpacing(0);
        m_vLayout->addWidget(m_toolBar);
        m_vLayout->addWidget(m_mainSplitter);
        m_vLayout->addWidget(m_bottomToolbar);


        m_centralWidget = new QWidget;
        m_centralWidget->setLayout(m_vLayout);

        setWindowTitle("NetAnim");

        initUpdateRate();
        setLabelStyleSheet();
        enableAllToolButtons(false);
        m_fileOpenButton->setEnabled(true);
        m_precisionButton->setChecked(true);
        m_fastFwdButton->setChecked(true);
        m_showMetaButton->setChecked(true);
        m_metaFontSpinBox->setValue(12);
        m_scene->setSceneInfoText("Please select an XML trace file using the file load button on the top-left corner", true);

}


QWidget *
AnimatorMode::getCentralWidget()
{
    return m_centralWidget;
}

void
AnimatorMode::metaFontSizeSlot(int value)
{
    m_scene->setMetaInfoFontSize(value);
}


void
AnimatorMode::showMetaSlot()
{
    m_scene->showMeta(m_showMetaButton->isChecked());
}

void
AnimatorMode::fastFwdSlot()
{
    m_fastFwd = m_fastFwdButton->isChecked();
    m_updateRateSlider->setEnabled(!m_fastFwdButton->isChecked());
    m_precisionButton->setChecked(!m_fastFwd);
    precisionSlot();
}
void
AnimatorMode::saveSlot()
{
    m_view->save();
}

 void
 AnimatorMode::precisionSlot()
 {
     //qDebug(QString("reset precision"));
     m_scene->setPacketPrecision(m_precisionButton->isChecked());
 }
 void
 AnimatorMode::showPacketSlot()
 {
    m_scene->blockPacketRendering(m_blockPacketsButton->isChecked());
    if(m_blockPacketsButton->isChecked())
        m_blockPacketsButton->setToolTip("Show packets");
    else
        m_blockPacketsButton->setToolTip("Don't Show packets");

 }


 void
 AnimatorMode::resetSlot()
 {
    m_scene->softReset();
    m_updateRateTimer->stop();
    m_currentTime = 0;
    m_timelineSpinBox->setValue(0);
    m_qLcdNumber->display(0);
    fflush(stdout);
    m_playing = false;
    m_updateRateTimer->stop();
    m_scene->invalidate();
    m_scene->timeToUpdate(0, m_fastFwd);
    m_playButton->setIcon(QIcon(":/animator_resource/animator_play.svg"));
    m_playButton->setToolTip("Play Animation");
    m_playButton->setEnabled(true);

 }

 void
 AnimatorMode::pktFilterFromSlot(QString nodeId)
 {
     if(nodeId == "All")
         m_scene->setPktFilterFromId(-1);
     else
         m_scene->setPktFilterFromId(nodeId.toUInt());
 }


 void
 AnimatorMode::pktFilterToSlot(QString nodeId)
 {
     if(nodeId == "All")
         m_scene->setPktFilterToId(-1);
     else
         m_scene->setPktFilterToId(nodeId.toUInt());

 }
 void
 AnimatorMode::timelineSlot(int value)
 {
    if(value == m_oldTimelineValue)
         return;
    m_oldTimelineValue = value;
    m_currentTime = value;
    if(m_nodePositionStatsButton->isChecked())
    {
        if(!m_playing)
            playSlot();
    }
    updateRateTimeoutSlot();

 }

 void
 AnimatorMode::setLabelStyleSheet()
 {
     QString labelStyleSheet = "QLabel {color: black; font: 10px}";
     m_nodeSizeLabel->setStyleSheet(labelStyleSheet);
     m_gridLinesLabel->setStyleSheet(labelStyleSheet);
     m_updateRateLabel->setStyleSheet(labelStyleSheet);
     m_packetPersistenceLabel->setStyleSheet(labelStyleSheet);
     m_timelineSpinBoxLabel->setStyleSheet(labelStyleSheet);
     m_metaFontLabel->setStyleSheet(labelStyleSheet);

 }

 void
 AnimatorMode::enableAllToolButtons(bool show)
 {
     for(int i = 0; i < m_toolButtonVector.size(); ++i)
     {
         m_toolButtonVector[i]->setEnabled(show);
     }

 }

 void
 AnimatorMode::initUpdateRate()
 {

     m_updateRates[0]  = 0.000001;
     m_updateRates[1]  = 0.000002;
     m_updateRates[2]  = 0.000004;
     m_updateRates[3]  = 0.000008;
     m_updateRates[4]  = 0.000016;
     m_updateRates[5]  = 0.000032;
     m_updateRates[6]  = 0.000064;
     m_updateRates[7]  = 0.000125;
     m_updateRates[8]  = 0.000250;
     m_updateRates[9]  = 0.000500;
     m_updateRates[10] = 0.001000;
     m_updateRates[11] = 0.002000;
     m_updateRates[12] = 0.004000;
     m_updateRates[13] = 0.008000;
     m_updateRates[14] = 0.016000;
     m_updateRates[15] = 0.032000;
     m_updateRates[16] = 0.064000;
     m_updateRates[17] = 0.125000;
     m_updateRates[18] = 0.250000;
     m_updateRates[19] = 0.500000;
     m_updateRates[20] = 1.000000;

     m_updateRateSlider->setValue(18);

     m_updateRateTimer = new QTimer(this);
     m_updateRateTimer->setInterval(m_currentUpdateRate*1000);
     connect(m_updateRateTimer, SIGNAL(timeout()), this, SLOT(updateRateTimeoutSlot()));


 }


 void
 AnimatorMode::nodePositionStatsSlot()
 {
     m_blockPacketsButton->click();

     if(m_nodePositionStatsButton->isChecked())
     {

         if(!m_scene->isNodeListEmpty())
         {
             m_nodePositionStatsButton->setEnabled(true);
             m_scene->showNodePosStats(m_nodePositionStatsButton->isChecked());
         }
         else
         {
             QMessageBox msg;
             msg.setText("No Node information was read. Did you load the XML trace file?");
             msg.exec();
             m_nodePositionStatsButton->setChecked(false);
             m_view->show();
         }
     }
     else
     {
         m_scene->showNodePosStats(m_nodePositionStatsButton->isChecked());
         m_view->show();
         enableAllToolButtons(true);
     }
 }

void
AnimatorMode::packetStatsSlot()
{
    if(m_packetStatsButton->isChecked())
    {
        if(!m_scene->isPacketListEmpty())
        {
            m_scene->getPacketStatsDialog()->resize(m_view->viewport()->size());
            m_scene->getPacketStatsDialog()->show();
            enableAllToolButtons(false);
            m_packetStatsButton->setEnabled(true);
            m_view->hide();
            m_scene->showPacketStats();
        }
        else
        {
            QMessageBox msg;
            msg.setText("No packets read. Did you load the XML trace file?");
            msg.exec();
            m_packetStatsButton->setChecked(false);
            m_view->show();
        }
    }
    else
    {
        m_scene->getPacketStatsDialog()->hide();
        m_view->show();
        enableAllToolButtons(true);
    }
}

 void
 AnimatorMode::showWirelessCirclesSlot()
 {
    m_showWiressCircles = !m_showWiressCircles;
    m_scene->showWirelessCircles(m_showWiressCircles);
    m_showWirelessCirclesButton->setChecked(m_showWiressCircles);
 }

 void
 AnimatorMode::zoomInSlot()
 {
    m_view->setCurrentZoomFactor(++m_currentZoomFactor);

 }

 void
 AnimatorMode::zoomOutSlot()
 {
     if(m_currentZoomFactor == 1)
         return;
     m_view->setCurrentZoomFactor(--m_currentZoomFactor);

 }

 void
 AnimatorMode::updateTimeLineSpinBox(int value)
 {
     disconnect(m_timelineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(timelineSlot(int)));
     m_timelineSpinBox->setValue(value);
     connect(m_timelineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(timelineSlot(int)));

 }

 void
 AnimatorMode::updateRateTimeoutSlot()
 {
     m_updateRateTimer->stop();
     if(m_playing)
     {
         if(m_appResponsiveTimer.elapsed() > 1000)
         {
            QApplication::processEvents();
            m_appResponsiveTimer.restart();
         }

         m_scene->timeToUpdate(m_currentTime, m_fastFwd);
         QString text = m_precisionButton->isChecked()? "": "Please Note: P2P link Packet precision is off";
         m_bottomStatusLabel->setText(text);
     }
     if(!m_fastFwd)
     {
        m_currentTime += m_currentUpdateRate;
     }
     else
     {
        double delta = 0;
        double futurePacketDelta = m_scene->getFuturePacketDelta();
        double futureNodeDelta = m_scene->getFutureNodeDelta();
        if(futurePacketDelta>=0)
            delta = futurePacketDelta;
        if(futureNodeDelta>=0)
        {
            if(delta)
            delta = std::min(delta, m_scene->getFutureNodeDelta());
            else
            delta = futureNodeDelta;
        }
        m_currentTime += delta;
        //qDebug(QString("FP:") + QString::number(futurePacketDelta) + " FN:" + QString::number(futureNodeDelta));
        m_updateRateTimer->setInterval(m_packetPersistTime);
        if(!delta)
         m_currentTime = m_parsedMaxSimulationTime + 0.001;
     }
     updateTimeLineSpinBox(m_currentTime);
     m_qLcdNumber->display(m_currentTime);
     fflush(stdout);
     if(m_playing)
     {
         m_updateRateTimer->start();
     }
     if(m_currentTime > m_parsedMaxSimulationTime)
     {
        m_updateRateTimer->stop();
        m_playButton->setEnabled(false);
        resetSlot();
        m_bottomStatusLabel->setText("Simulation Completed");
        m_simulationCompleted = true;

     }

 }

 void
 AnimatorMode::updateRateChangedSlot(int value)
 {
     //qDebug("update rate changed");
     m_currentUpdateRate = m_updateRates[value];
     m_updateRateLabel->setText(QString::number(m_currentUpdateRate*1000)+"ms");
     if(m_updateRateTimer)
     m_updateRateTimer->setInterval(m_currentUpdateRate*1000);
 }


 bool
 AnimatorMode::parseXMLTraceFile(QString traceFileName)
 {
     Animxmlparser parser(traceFileName);
     if(!parser.isFileValid())
     {
         QMessageBox msg;
         msg.setText("Trace file is invalid");
         msg.exec();
         m_fileOpenButton->setEnabled(true);
         return false;
     }
     if(!m_parsingXMLDialog)
     {
         m_parsingXMLDialog = new QDialog(this);
         m_parsingXMLDialog->setWindowTitle("Parsing XML trace file");
         QVBoxLayout * vboxLayout  = new QVBoxLayout;
         vboxLayout->addWidget(new QLabel("Please Wait.Parsing XML trace file"));
         m_parsingXMLDialog->setLayout(vboxLayout);
     }
     m_parsingXMLDialog->show();
     m_parsingXMLDialog->raise();
     m_parsingXMLDialog->activateWindow();

     m_scene->setSceneInfoText(QString::number(parser.getRxCount()), true);
     m_parseProgressBar->setMaximum(parser.getRxCount());
     m_parseProgressBar->setVisible(true);
     uint64_t gParsedElementCount = 0;
     while(!parser.isParsingComplete())
     {
        if(!m_parsingXMLDialog->isVisible())
            break;
        if(m_appResponsiveTimer.elapsed() > 1000)
         {
            QApplication::processEvents();
            m_bottomStatusLabel->setText("Parsing Count:" + QString::number(gParsedElementCount));
            m_appResponsiveTimer.restart();
        }
        ParsedElement parsedElement = parser.parseNext();
        switch(parsedElement.type)
        {

            case XML_TOPOLOGY:
            {
                qreal maxValue = qMax(parsedElement.topo_width, parsedElement.topo_height);
                m_scene->width(maxValue);
                m_scene->height(maxValue);
                break;
            }
            case XML_NODE:
            {
                m_scene->addNode(parsedElement.nodeId,
                               parsedElement.node_x,
                               parsedElement.node_y,
                               CIRCLE,
                               nodeSizeStringToValue(m_nodeSizeComboBox->currentText()),
                               nodeSizeStringToValue(m_nodeSizeComboBox->currentText()),
                               parsedElement.nodeDescription);
                break;
            }
            case XML_LINK:
            {
                m_scene->addLink(parsedElement.link_fromId,
                               parsedElement.link_toId);
                break;
            }
            case XML_PACKET_RX:
            case XML_WPACKET_RX:
            {
                m_scene->addPacketRx(parsedElement.packetrx_fromId,
                                   parsedElement.packetrx_fbTx,
                                   parsedElement.packetrx_lbTx,
                                   parsedElement.packetrx_toId,
                                   parsedElement.packetrx_fbRx,
                                   parsedElement.packetrx_lbRx,
                                   parsedElement.type,
                                   parsedElement.meta_info);
                ++gParsedElementCount;
                m_parseProgressBar->setValue(gParsedElementCount);

                break;
            }
            case XML_INVALID:
            default:
            {
                //qDebug("Invalid XML element");
            }


        } //switch

     } // while loop

     //setGeometry(0, 0, m_view->width(), m_view->height());
     m_view->squareViewPort();

     enableAllToolButtons(true);
     resetSlot();
     m_showNodeIdButton->setChecked(true);
     showNodeIdSlot();

     QStringList nodeList;
     for(uint32_t i = 0; i < m_scene->getNodeCount(); ++i)
     {
         nodeList << QString::number(i);
     }
     m_scene->updateP2pLinks();
     m_parsedMaxSimulationTime = parser.getMaxSimulationTime();
     m_timelineSpinBox->setRange(0, (m_parsedMaxSimulationTime));
     m_parsingXMLDialog->hide();
     fastFwdSlot();
     m_gridButton->setChecked(true);
     gridSlot();
     m_scene->setSceneInfoText("", false);
     m_bottomStatusLabel->setText("Parsing complete");
     m_parseProgressBar->reset();
     return true;
 }


 void
 AnimatorMode::traceFileOpenSlot()
 {

    systemReset();
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    QString traceFileName ="";
    if(fileDialog.exec())
    {
        traceFileName = fileDialog.selectedFiles().at(0);
        //qDebug((traceFileName));
        if(traceFileName != "")
        {
            if(parseXMLTraceFile(traceFileName))
                m_fileOpenButton->setEnabled(true);
        }
    }
    m_zoomInButton->click();
    QApplication::processEvents();
    m_zoomOutButton->click();
    QApplication::processEvents();
    m_nodeSizeComboBox->setCurrentIndex(7);
    nodeSizeSlot("300%");


 }

 void
 AnimatorMode::playSlot()
 {
    m_playing = !m_playing;
    if(m_playing)
    {
        if(m_simulationCompleted)
        {
            m_scene->softReset();
            m_simulationCompleted = false;
        }
        m_appResponsiveTimer.restart();
        m_scene->setNodeSize(nodeSizeStringToValue(m_nodeSizeComboBox->currentText()));
        m_scene->prepareForPlay();
        m_playButton->setIcon(QIcon(":/animator_resource/animator_pause.svg"));
        m_playButton->setToolTip("Pause Animation");
        if(m_fastFwd)
        {
            m_updateRateTimer->setInterval(0);
        }

        m_updateRateTimer->start();

    }
    else
    {
        m_playButton->setIcon(QIcon(":/animator_resource/animator_play.svg"));
        m_playButton->setToolTip("Play Animation");
        m_updateRateTimer->stop();
    }
 }


 void
 AnimatorMode::testSlot()
 {

    m_scene->test();
 }


 void
 AnimatorMode::systemReset()
 {
    m_packetStatsButton->setChecked(false);
    packetStatsSlot();
    m_updateRateTimer->stop();
    m_view->systemReset();
    m_scene->systemReset();
    resetSlot();
    m_showNodeIdButton->click();
    m_scene->setSceneInfoText("Please select an XML trace file using the file load button on the top-left corner", true);

 }

 qreal
 AnimatorMode::nodeSizeStringToValue(QString nodeSize)
 {
     if(nodeSize == "20%")
         return 0.2;
     if(nodeSize == "40%")
         return 0.4;
     if(nodeSize == "50%")
         return 0.5;
     if(nodeSize == "60%")
         return 0.6;
     if(nodeSize == "80%")
         return 0.8;
     if(nodeSize == "")
         return 1;
     if(nodeSize == "200%")
         return 2;
     if(nodeSize == "300%")
         return 3;
     if(nodeSize == "400%")
         return 4;
     if(nodeSize == "500%")
         return 5;
     if(nodeSize == "600%")
         return 6;
     if(nodeSize == "900%")
         return 9;
     if(nodeSize == "1000%")
         return 10;
     if(nodeSize == "2000%")
         return 20;
     return 1;

 }

 void
 AnimatorMode::nodeSizeSlot(QString value)
 {
    m_scene->setNodeSize(nodeSizeStringToValue(value));

 }

 void
 AnimatorMode::packetPersistenceSlot(QString value)
 {
    /* persistTimes << "10ms"
                  << "50ms"
                  << "100ms"
                  << "250ms"
                  << "500ms"
                  << "1s";*/
     if(value == "10ms")
         m_packetPersistTime = 10;
     if(value == "50ms")
         m_packetPersistTime = 50;
     if(value == "100ms")
         m_packetPersistTime = 100;
     if(value == "250ms")
         m_packetPersistTime = 250;
     if(value == "500ms")
         m_packetPersistTime = 500;
     if(value == "1s")
         m_packetPersistTime = 1000;

 }

 void
 AnimatorMode::showNodeIdSlot()
 {
    m_showNodeId = !m_showNodeId;
    m_scene->showNodeId(m_showNodeIdButton->isChecked());
    if(m_showNodeId)
    {
        m_showNodeIdButton->setToolTip("Don't show Node Id");
    }
    else
    {
        m_showNodeIdButton->setToolTip("Show Node Id");
    }
 }



 void
 AnimatorMode::gridSlot()
 {
     m_showGrid = m_gridButton->isChecked();
     m_scene->showGrid(m_showGrid);
     if(m_showGrid)
     {
         m_gridButton->setToolTip("Turn OFF Grid");
         m_gridButton->setChecked(true);
     }
     else
     {
         m_gridButton->setToolTip("Turn ON Grid");
         m_gridButton->setChecked(false);
     }
 }


 void
 AnimatorMode::gridLinesSlot(int value)
 {
    m_scene->setGridLinesCount(value);
 }



 QString
 AnimatorMode::getTabName()
 {
     return "Animator";
 }


} // namespace netanim
