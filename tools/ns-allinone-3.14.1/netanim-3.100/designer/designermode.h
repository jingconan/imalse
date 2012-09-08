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

#ifndef DESIGNERMODE_H
#define DESIGNERMODE_H

#include "designerview.h"
#include "designerscene.h"
#include <QWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>

namespace netanim {

class DesignerMode : public QWidget
{
    Q_OBJECT
public:
    explicit DesignerMode(QWidget * parent = 0);
    QWidget * getCentralWidget();
    QString getTabName();

private:
    QWidget * m_centralWidget;
    QToolBar * m_verticalToolBar;
    QToolBar * m_horizontalToolBar;
    QHBoxLayout * m_horizontalLayout;
    QVBoxLayout * m_layout;
    DesignerView * m_designerView;
    DesignerScene * m_designerScene;


    QLabel * m_canvasWidthLabel;
    QSpinBox * m_canvasWidthSpinBox;

    QLabel * m_canvasHeightLabel;
    QSpinBox * m_canvasHeightSpinBox;

    QLabel * m_nodeSizeLabel;
    QSpinBox * m_nodeSizeSpinBox;



    QToolButton * m_nodeButton;
    QToolButton * m_linkButton;
    QToolButton * m_nodeMoveButton;

    QToolButton * m_linkManageButton;
    QToolButton * m_appManageButton;

    QToolButton * m_playButton;

    QVector <QToolButton *> m_toolButtonGroup;



    void initStockToolbar();
    void unCheckToolButtonGroup(QToolButton * skip);


signals:

public slots:


private slots:
    void canvasWidthSlot(int value);
    void canvasHeightSlot(int value);
    void nodeSizeSlot(int value);
    void nodeSlot();
    void linkSlot();
    void nodeMoveSlot();
    void linkManageSlot();
    void appManageSlot();
    void playSlot();


};

} // namespace netanim
#endif // DESIGNERMODE_H
