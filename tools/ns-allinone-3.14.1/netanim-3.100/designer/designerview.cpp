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
#include "designerview.h"

namespace netanim {

DesignerView::DesignerView(QGraphicsScene * scene) :
    QGraphicsView(scene),
    m_currentZoomFactor(1)
{
}


void
DesignerView::paintEvent(QPaintEvent * event)
{
    QFont f;
    f.setPointSize(5); // TODO move to designer constant
    setFont(f);
    setRenderHint(QPainter::Antialiasing);
    QTransform transform;
    qreal xScale = rect().width()/sceneRect().width();
    qreal yScale = rect().height()/sceneRect().height();
    transform.scale(xScale*m_currentZoomFactor, yScale*m_currentZoomFactor);
    setTransform(transform);
    getDesignerScene()->setCurrentScale(xScale, yScale);
    //qDebug(transform);
    QGraphicsView::paintEvent(event);

}


DesignerScene *
DesignerView::getDesignerScene()
{
    return (DesignerScene *) (scene());

}

} // namespace netanim
