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

#include "attributeipv4addrportwidget.h"

namespace netanim {

AttributeIpv4AddrPortWidget::AttributeIpv4AddrPortWidget(QWidget *parent) :
    QWidget(parent)
{
    m_hLayout = new QHBoxLayout;

    m_ipv4AddrLabel = new QLabel("Ipv4 Address");
    m_ipv4AddrLineEdit = new QLineEdit("");

    m_portLabel = new QLabel("Port");
    m_portLineEdit = new QLineEdit("");

    m_hLayout->addWidget(m_ipv4AddrLabel);
    m_hLayout->addWidget(m_ipv4AddrLineEdit);
    m_hLayout->addWidget(m_portLabel);
    m_hLayout->addWidget(m_portLineEdit);
    setLayout(m_hLayout);
}


ns3::AddressValue
AttributeIpv4AddrPortWidget::getAddressValue()
{

    return (ns3::AddressValue(
                ns3::InetSocketAddress(
                    m_ipv4AddrLineEdit->text().toAscii().data(),
                    m_portLineEdit->text().toUShort()
                )));
}

void
AttributeIpv4AddrPortWidget::setIpv4Addr(QString ipv4Addr)
{
    m_ipv4AddrLineEdit->setText(ipv4Addr);
}

void
AttributeIpv4AddrPortWidget::setPort(QString port)
{
    m_portLineEdit->setText(port);
}

} // namespace netanim
