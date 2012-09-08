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


#ifndef ATTRIBUTEPROTOCOLWIDGET_H
#define ATTRIBUTEPROTOCOLWIDGET_H

#include "ns3/core-module.h"

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>

namespace netanim {


class AttributeProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttributeProtocolWidget(QWidget *parent = 0);
    ns3::TypeIdValue getTypeIdValue();
    void setTypeIdValue(ns3::TypeIdValue tv);

private:
    QHBoxLayout * m_hLayout;
    QComboBox * m_protocolsComboBox;

    void addSupportedProtocols();

signals:

public slots:

};

} // namespace netanim

#endif // ATTRIBUTEPROTOCOLWIDGET_H
