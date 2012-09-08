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
#include "designerp2plinkline.h"

namespace netanim {

DesignerP2PLinkLine::DesignerP2PLinkLine():
    m_srcNodeItemId(0),
    m_dstNodeItemId(0),
    m_dataRate(QString("5Mbps")),
    m_delay(QString("50ms")),
    m_ipv4Base(QString("10.1.1.0")),
    m_ipv4Mask(QString("255.255.255.0"))
{
}


AbstractDesignerItem::ItemType_t
DesignerP2PLinkLine::getItemType()
{
    return P2PLinkLine;
}


uint64_t
DesignerP2PLinkLine::getSrcNodeItemId()
{
    return m_srcNodeItemId;
}


uint64_t
DesignerP2PLinkLine::getDstNodeItemId()
{
    return m_dstNodeItemId;
}

void
DesignerP2PLinkLine::setSrcNodeItemId(uint64_t itemId)
{
    m_srcNodeItemId = itemId;
}


void
DesignerP2PLinkLine::setDstNodeItemId(uint64_t itemId)
{
    m_dstNodeItemId = itemId;
}



void
DesignerP2PLinkLine::setDelay(QString delay)
{
    m_delay = delay;
}

void
DesignerP2PLinkLine::setDataRate(QString dataRate)
{
    m_dataRate = dataRate;

}


QString
DesignerP2PLinkLine::getDataRate()
{
    return m_dataRate;
}

QString
DesignerP2PLinkLine::getDelay()
{
    return m_delay;
}

void
DesignerP2PLinkLine::setIpv4Base(QString ipv4Base)
{
    m_ipv4Base = ipv4Base;
}

void
DesignerP2PLinkLine::setIpv4Mask(QString ipv4Mask)
{
    m_ipv4Mask = ipv4Mask;
}

QString
DesignerP2PLinkLine::getIpv4Base()
{
    return m_ipv4Base;
}

QString
DesignerP2PLinkLine::getIpv4Mask()
{
    return m_ipv4Mask;
}

} // namespace netanim
