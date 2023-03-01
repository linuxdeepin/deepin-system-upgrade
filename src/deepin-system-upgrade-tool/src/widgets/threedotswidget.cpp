// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QPainter>
#include <QBrush>

#include "threedotswidget.h"

ThreeDotsWidget::ThreeDotsWidget(QWidget *parent, int aDot)
    : QWidget(parent)
    , m_activeDot(aDot)
    , m_activeColor(QColor("#BF0081FF"))
    , m_deactiveColor(QColor("#260081FF"))
{
    setFixedSize(40, 8);
}

void ThreeDotsWidget::paintEvent(QPaintEvent *event)
{
    initUI();
}

void ThreeDotsWidget::initUI()
{
    QPainter painter(this);
    // 必须要设置画边界的Pen，否则画出来的圆是扁的。
    QPen borderPen(QColor("#00000000"));
    painter.setPen(borderPen);

    QBrush activeBrush(m_activeColor), deactiveBrush(m_deactiveColor);

    for (int i = 0; i < 3; i++)
    {
        // 使用两个不同Painter画会只能画后面一个QBrush的颜色。所以需要实时换QBrush。
        if (i == m_activeDot)
        {
            painter.setBrush(activeBrush);
        }
        else
        {
            painter.setBrush(deactiveBrush);
        }
        painter.drawEllipse(14 * i, 0, 6, 6);
    }
}
