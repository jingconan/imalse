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
#ifndef ABSTRACTDESIGNERITEM_H
#define ABSTRACTDESIGNERITEM_H

#include <Qt>

namespace netanim {

class AbstractDesignerItem
{
public:
    AbstractDesignerItem();
    typedef enum {Invalid, NodeRect, P2PLinkLine} ItemType_t;
    uint64_t getDesignerItemId();
    virtual ItemType_t getItemType()=0;
private:
    uint64_t m_designerItemId;
};

} // namespace netanim
#endif // ABSTRACTDESIGNERITEM_H
