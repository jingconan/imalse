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


#include "packetstatisticsdialog.h"
#include <QCloseEvent>
#include <QMessageBox>

namespace netanim {

Packetstatisticsdialog::Packetstatisticsdialog():
    m_allowClose(true)
{
    hide();
}

void
Packetstatisticsdialog::closeEvent(QCloseEvent *event)
{
    if(m_allowClose)
        QDialog::closeEvent(event);
    else
    {
        QMessageBox msg;
        msg.setText("Please Wait. Parsing in progress");
        msg.exec();
        event->ignore();
    }
}

void
Packetstatisticsdialog::setAllowClose(bool allow)
{
    m_allowClose = allow;
}

} // namespace netanim
