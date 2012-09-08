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

#ifndef ATTRIBUTEIPV4ADDRPORTWIDGET_H
#define ATTRIBUTEIPV4ADDRPORTWIDGET_H

#include "ns3/network-module.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace netanim {

class AttributeIpv4AddrPortWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttributeIpv4AddrPortWidget(QWidget *parent = 0);
    ns3::AddressValue getAddressValue();
    void setIpv4Addr(QString ipv4Addr);
    void setPort(QString port);
private:
    QHBoxLayout * m_hLayout;
    QLabel * m_ipv4AddrLabel;
    QLineEdit * m_ipv4AddrLineEdit;

    QLabel * m_portLabel;
    QLineEdit * m_portLineEdit;


signals:

public slots:

};

} // namespace netanim
#endif // ATTRIBUTEIPV4ADDRPORTWIDGET_H
