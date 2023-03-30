// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <DHeaderView>

#include <QObject>
#include <QWidget>

DWIDGET_USE_NAMESPACE

class BorderRadiusHeaderView : public DHeaderView
{
public:
    BorderRadiusHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
     virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void paintEvent(QPaintEvent *event) override;
};
