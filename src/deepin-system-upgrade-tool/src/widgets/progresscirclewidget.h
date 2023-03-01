// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGuiApplication>
#include <QTimer>
#include <QIcon>
#include <QPixmap>
#include <QScreen>
#include <QWidget>
#include <QColor>

class ProgressCircleWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ProgressCircleWidget(QWidget *parent=nullptr);
    void setValue(qint64 v);
    void start();
    void stop();

protected:
    void paintEvent(QPaintEvent *event);

private:
    // 进度值
    qint64          m_value = 0;
    // 单位：Milisecond, 屏幕刷新间隔
    const int       kRefreshInterval = static_cast<int>(1000 / QGuiApplication::primaryScreen()->refreshRate());
    const int       kDegreePerSec = static_cast<int>(360 * 0.7);
    const int       kArcR = 160 / 2;
    // 外圈圆弧旋转角度
    qreal           m_degree;
    QTimer          *m_animTimer;
    const QPixmap   kBackgroundPixmap = QIcon(":/icons/circle_background.svg").pixmap(152, 152);
    const QPixmap   kArcPixmap = QIcon(":/icons/gradient_arc.svg").pixmap(160, 160);

    void initUI();
};
