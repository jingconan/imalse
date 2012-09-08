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

#include "attributeprotocolwidget.h"
#include "debug/xdebug.h"
#include "ns3/internet-module.h"

namespace netanim {

AttributeProtocolWidget::AttributeProtocolWidget(QWidget *parent) :
    QWidget(parent)
{
    m_hLayout = new QHBoxLayout;
    m_protocolsComboBox = new QComboBox;
    m_hLayout->addWidget(m_protocolsComboBox);
    setLayout(m_hLayout);
    addSupportedProtocols();
}


void
AttributeProtocolWidget::addSupportedProtocols()
{
    m_protocolsComboBox->addItem("Udp");
    m_protocolsComboBox->addItem("Tcp");
}


ns3::TypeIdValue
AttributeProtocolWidget::getTypeIdValue()
{
    if(m_protocolsComboBox->currentText() == "Udp")
        return ns3::TypeIdValue(ns3::UdpSocketFactory::GetTypeId());
    else if(m_protocolsComboBox->currentText() == "Tcp")
        return ns3::TypeIdValue(ns3::TcpSocketFactory::GetTypeId());
    else
        return ns3::TypeIdValue();

}

void
AttributeProtocolWidget::setTypeIdValue(ns3::TypeIdValue tv)
{
    std::string name = tv.Get().GetName();
    if(name == "ns3::TcpSocketFactory")
        m_protocolsComboBox->setCurrentIndex(m_protocolsComboBox->findText("Tcp"));
    else if(name == "ns3::UdpSocketFactory")
        m_protocolsComboBox->setCurrentIndex(m_protocolsComboBox->findText("Udp"));
}

} // namespace netanim
