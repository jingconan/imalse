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
#include "netanim.h"
#include "animator/animatormode.h"

#ifdef WITH_NS3
#include "designer/designermode.h"
#endif

#include <QApplication>
#include <QDesktopWidget>

namespace netanim {

NetAnim::NetAnim():
    m_tabWidget(new QTabWidget)
{


    AnimatorMode * animatorTab = new AnimatorMode;
    m_tabWidget->addTab(animatorTab->getCentralWidget(), animatorTab->getTabName());

#ifdef WITH_NS3
    DesignerMode * designerMode = new DesignerMode;
    m_tabWidget->addTab(designerMode->getCentralWidget(), designerMode->getTabName());
#endif
    m_tabWidget->setCurrentIndex(1);
    QRect geometry = QApplication::desktop()->screenGeometry();
    int minimumDimension = std::min(geometry.width(), geometry.height());
    m_tabWidget->setGeometry(0, 0, minimumDimension, minimumDimension);

    m_tabWidget->show();
}

} // namespace netanim
