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
#include "designerconstants.h"
#include "designermode.h"

namespace netanim {

DesignerMode::DesignerMode(QWidget *parent) :
    QWidget(parent)
{

    m_designerScene = new DesignerScene;
    m_designerView = new DesignerView(m_designerScene);
    m_verticalToolBar = new QToolBar;
    m_verticalToolBar->setOrientation(Qt::Vertical);

    m_horizontalToolBar = new QToolBar;
    m_horizontalLayout = new QHBoxLayout;
    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_horizontalToolBar);
    m_layout->addLayout(m_horizontalLayout);
    m_horizontalLayout->addWidget(m_verticalToolBar);
    m_horizontalLayout->addWidget(m_designerView);



    // Horizontal Toolbar elements
    m_canvasWidthLabel = new QLabel("Width");
    m_canvasWidthSpinBox = new QSpinBox;
    m_canvasWidthSpinBox->setRange(1, 2500);
    m_canvasWidthSpinBox->setValue(1024);
    m_canvasWidthSpinBox->setToolTip("Canvas Width");
    connect(m_canvasWidthSpinBox,SIGNAL(valueChanged(int)),this,SLOT(canvasWidthSlot(int)));

    m_canvasHeightLabel = new QLabel("Height");
    m_canvasHeightSpinBox = new QSpinBox;
    m_canvasHeightSpinBox->setRange(1, 2500);
    m_canvasHeightSpinBox->setValue(1024);
    m_canvasHeightSpinBox->setToolTip("Canvas Height");
    connect(m_canvasHeightSpinBox,SIGNAL(valueChanged(int)),this,SLOT(canvasHeightSlot(int)));


    m_nodeSizeLabel = new QLabel("Node size");
    m_nodeSizeSpinBox = new QSpinBox;
    m_nodeSizeSpinBox->setRange(1, 5000);
    m_nodeSizeSpinBox->setValue(DEFAULT_NODE_SIZE);
    m_nodeSizeSpinBox->setToolTip("Node Size");
    connect(m_nodeSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(nodeSizeSlot(int)));

    m_playButton = new QToolButton;
    m_playButton->setIcon(QIcon(":/animator_resource/animator_play.svg"));
    m_playButton->setGeometry(0, 0, 10, 20);
    m_playButton->setToolTip("Play Simualtion");
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playSlot()));


    m_horizontalToolBar->addWidget(m_canvasWidthLabel);
    m_horizontalToolBar->addWidget(m_canvasWidthSpinBox);
    m_horizontalToolBar->addWidget(m_canvasHeightLabel);
    m_horizontalToolBar->addWidget(m_canvasHeightSpinBox);
    m_horizontalToolBar->addWidget(m_nodeSizeLabel);
    m_horizontalToolBar->addWidget(m_nodeSizeSpinBox);
    m_horizontalToolBar->addWidget(m_playButton);

    m_layout->setSpacing(0);
    initStockToolbar();
    m_centralWidget = new QWidget;
    m_centralWidget->setLayout(m_layout);

    m_nodeMoveButton->click();
    m_nodeButton->click();
    m_nodeButton->click();
    m_linkButton->click();
}

void
DesignerMode::initStockToolbar()
{

    m_nodeButton = new QToolButton;
    m_nodeButton->setText("A");
    m_nodeButton->setToolTip("Add Node to canvas");
    connect(m_nodeButton,SIGNAL(clicked()),this,SLOT(nodeSlot()));


    m_linkButton = new QToolButton;
    m_linkButton->setText("L");
    m_linkButton->setToolTip("Add Link to canvas");
    m_linkButton->setCheckable(true);
    connect(m_linkButton,SIGNAL(clicked()),this,SLOT(linkSlot()));

    m_nodeMoveButton = new QToolButton;
    m_nodeMoveButton->setText("M");
    m_nodeMoveButton->setToolTip("Move nodes");
    m_nodeMoveButton->setCheckable(true);
    connect(m_nodeMoveButton,SIGNAL(clicked()),this,SLOT(nodeMoveSlot()));


    m_linkManageButton = new QToolButton;
    m_linkManageButton->setText("LM");
    m_linkManageButton->setToolTip("Manage Links");
    connect(m_linkManageButton,SIGNAL(clicked()),this,SLOT(linkManageSlot()));


    m_appManageButton = new QToolButton;
    m_appManageButton->setText("AM");
    m_appManageButton->setToolTip("Manage Apps");
    connect(m_appManageButton,SIGNAL(clicked()),this,SLOT(appManageSlot()));


    m_verticalToolBar->addWidget(m_nodeButton);
    m_verticalToolBar->addWidget(m_linkButton);
    m_verticalToolBar->addWidget(m_nodeMoveButton);
    m_verticalToolBar->addWidget(m_linkManageButton);
    m_verticalToolBar->addWidget(m_appManageButton);


    m_toolButtonGroup.push_back(m_linkButton);
    m_toolButtonGroup.push_back(m_nodeMoveButton);
}



void
DesignerMode::nodeMoveSlot()
{
    unCheckToolButtonGroup(m_nodeMoveButton);

    if(m_nodeMoveButton->isChecked())
        m_designerScene->setDesignerSceneMode(DesignerScene::NodeMove);
    else
        m_designerScene->setDesignerSceneMode(DesignerScene::Normal);
}


void
DesignerMode::linkSlot()
{
    unCheckToolButtonGroup(m_linkButton);
    if(m_linkButton->isChecked())
        m_designerScene->setDesignerSceneMode(DesignerScene::P2PLinkLine);
    else
        m_designerScene->setDesignerSceneMode(DesignerScene::Normal);
}

void
DesignerMode::linkManageSlot()
{
    m_designerScene->showLinkManager();
}


void
DesignerMode::appManageSlot()
{
    m_designerScene->showAppManager();
}

void
DesignerMode::nodeSizeSlot(int value)
{
    m_designerScene->setNodeSize(value);

}

QWidget *
DesignerMode::getCentralWidget()
{
    return m_centralWidget;
}

QString
DesignerMode::getTabName()
{
    return "Designer";
}

void
DesignerMode::canvasWidthSlot(int value)
{
    m_designerScene->setCanvasWidth(value);
}

void
DesignerMode::canvasHeightSlot(int value)
{
    m_designerScene->setCanvasHeight(value);
}


void
DesignerMode::nodeSlot()
{
   // qDebug("Node pressed");
    m_designerScene->addNewNode();
}

void
DesignerMode::unCheckToolButtonGroup(QToolButton * skip)
{

    for(int i = 0; i < m_toolButtonGroup.size() ; i++)
    {
        if(skip == m_toolButtonGroup[i])
            continue;
        m_toolButtonGroup[i]->setChecked(false);
    }
}

void
DesignerMode::playSlot()
{
    m_designerScene->play();
}

} // namespace netanim
