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
#ifndef DESIGNERP2PLINKLINE_H
#define DESIGNERP2PLINKLINE_H

#include "abstractdesigneritem.h"
#include <QtGui/QGraphicsLineItem>

namespace netanim {

class DesignerP2PLinkLine : public QGraphicsLineItem, public AbstractDesignerItem
{
public:
    DesignerP2PLinkLine();
    uint64_t getSrcNodeItemId();
    uint64_t getDstNodeItemId();
    void setSrcNodeItemId(uint64_t itemId);
    void setDstNodeItemId(uint64_t itemId);
    void setDelay(QString delay);
    void setDataRate(QString dataRate);
    QString getDelay();
    QString getDataRate();
    void setIpv4Base(QString ipv4Base);
    void setIpv4Mask(QString ipv4Mask);
    QString getIpv4Base();
    QString getIpv4Mask();
private:
    ItemType_t getItemType();
    uint64_t m_srcNodeItemId;
    uint64_t m_dstNodeItemId;

    QString m_dataRate;
    QString m_delay;
    QString m_ipv4Base;
    QString m_ipv4Mask;
};

} // namespace netanim

#endif // DESIGNERP2PLINKLINE_H
