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


#include "animatorview.h"
#include "animatorscene.h"
#include "debug/xdebug.h"

#include <QtGui/QGraphicsScene>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QSvgGenerator>

namespace netanim {

AnimatorView::AnimatorView(QGraphicsScene * scene) :
    QGraphicsView(scene),
    m_currentZoomFactor(1)
{
    setRenderHint(QPainter::Antialiasing);
    //setCacheMode(QGraphicsView::CacheNone);
}

void
AnimatorView::paintEvent(QPaintEvent *event)
{
    //qDebug(transform);
    QGraphicsView::paintEvent(event);
}


void
AnimatorView::resizeEvent(QResizeEvent *event)
{
    updateTransform();
    QGraphicsView::resizeEvent(event);
}

void
AnimatorView::mouseMoveEvent(QMouseEvent * event)
{
    scene()->invalidate();
    QGraphicsView::mouseMoveEvent(event);
}


AnimatorScene *
AnimatorView::getAnimatorScene()
{
    return  (AnimatorScene *) (scene());
}

void
AnimatorView::updateTransform()
{
    QTransform transform;
    qreal xScale = rect().width()/sceneRect().width();
    qreal yScale = rect().height()/sceneRect().height();
    transform.scale(xScale*m_currentZoomFactor, yScale*m_currentZoomFactor);
    getAnimatorScene()->setCurrentScale(xScale*m_currentZoomFactor, yScale*m_currentZoomFactor);
    setTransform(transform);

}

void
AnimatorView::setCurrentZoomFactor(int factor)
{
    m_currentZoomFactor = factor;
    updateTransform();
    update();
}

void
AnimatorView::save()
{
    QString fileName = "";
    fileName = QFileDialog::getSaveFileName();
    if(fileName=="")
        return;
    QSvgGenerator svgGen;
    svgGen.setFileName(fileName);
    svgGen.setSize(QSize(size()));
    QPainter painter(&svgGen);
    painter.setRenderHint(QPainter::Antialiasing);
    render(&painter);
}

void
AnimatorView::systemReset()
{
    m_currentZoomFactor = 1;

}

void
AnimatorView::squareViewPort()
{
    qreal maxValue = qMax(width(), height());
    setGeometry(0, 0, maxValue, maxValue);
}


} // namespace netanim
