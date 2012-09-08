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

#ifndef NODEPROPERTIESTAB_H
#define NODEPROPERTIESTAB_H

#include "nodenetworkwidget.h"
#include "applicationswidget.h"
#include <QtGui/QDialog>
#include <QtGui/QTableWidget>

namespace netanim {

class NodePropertiesTab : public QDialog
{
    Q_OBJECT
public:
    NodePropertiesTab(void * nodeItem,
                      void * linkManager,
                      void * appManager,
                      QWidget * parent = 0);

private:
    QTabWidget * m_tabWidget;
    NodeNetworkWidget * m_nodeNetworkWidget;
    void * m_nodeItem;
    void * m_linkManager;

    Applicationswidget * m_applicationsWidget;
    void * m_appManager;


    void addP2PDevices(void * nodeItem, void * linkManager);
protected:
    void paintEvent(QPaintEvent * event);
};

} // namespace netanim
#endif // NODEPROPERTIESTAB_H
