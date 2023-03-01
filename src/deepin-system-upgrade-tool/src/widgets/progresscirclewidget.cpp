// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QColor>
#include <QDebug>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QRect>
#include <QtMath>

#include "progresscirclewidget.h"


ProgressCircleWidget::ProgressCircleWidget(QWidget *parent)
    : QWidget(parent)
    , m_animTimer(new QTimer(this))
{
    initUI();
    connect(m_animTimer, &QTimer::timeout, [this] {
        m_degree += kDegreePerSec * kRefreshInterval / 1000.;
        this->update();
    });
}

void ProgressCircleWidget::initUI()
{
    m_value = 0;
    m_animTimer->setInterval(kRefreshInterval);
}

void ProgressCircleWidget::setValue(qint64 v)
{
    if (v != m_value) {
        m_value = v;
        update();
    }
}

void ProgressCircleWidget::start() {
    m_degree = 0.;
    m_animTimer->start();
}

void ProgressCircleWidget::stop() {
    m_degree = 0.;
    m_animTimer->stop();
}

void ProgressCircleWidget::paintEvent(QPaintEvent *event)
{
    QRect rectValue;
    QPainter painter(this);
    painter.setFont(QFont("SourceHanSansSC", 42, QFont::Bold));
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置中心圆渐变色
    painter.drawPixmap(4, 4, kBackgroundPixmap);

    // 设置外圈圆弧
    // 设置坐标系到正确位置，做中心旋转
    painter.translate(kArcR, kArcR);
    painter.rotate(m_degree);
    painter.translate(-kArcR, -kArcR);

    painter.drawPixmap(0, 0, kArcPixmap);

    // 恢复原本的坐标系
    painter.translate(kArcR, kArcR);
    painter.rotate(-m_degree);
    painter.translate(-kArcR, -kArcR);

    painter.setPen(QColor("#FF001A2E"));
    // 对一位和两位百分比数设置不同居中效果
    if (m_value >= 10)
        painter.drawText(48, 100, QString::number(m_value));
    else
        painter.drawText(64, 100, QString::number(m_value));

    // 画"%"
    painter.setFont(QFont("SourceHanSansSC", 10, QFont::Normal));
    painter.drawText(114, 42, QString("%"));
}
