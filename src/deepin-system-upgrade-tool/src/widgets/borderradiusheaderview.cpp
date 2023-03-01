// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DPalette>
#include <DPaletteHelper>
#include <DApplicationHelper>
#include <DApplication>
#include <DStyle>

#include <QPainter>
#include <QPainterPath>

#include "../core/utils.h"
#include "borderradiusheaderview.h"

static const int kSpacingMargin = 4;

BorderRadiusHeaderView::BorderRadiusHeaderView(Qt::Orientation orientation, QWidget *parent)
    : DHeaderView (orientation, parent)
{
    viewport()->setAutoFillBackground(false);
}

void BorderRadiusHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setOpacity(1);

        DPalette::ColorGroup cg = DPalette::Active;
        DPalette palette = DApplicationHelper::instance()->applicationPalette();
        DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
        if (!style)
            return;

        QStyleOptionHeader option;
        initStyleOption(&option);
        int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);

        // title
        QRect contentRect(rect.x(), rect.y(), rect.width(), rect.height() - 1);
        QRect hSpacingRect(rect.x(), contentRect.height(), rect.width(),
                           rect.height() - contentRect.height());

        QBrush contentBrush(palette.color(cg, DPalette::Base));
        // horizontal spacing
        QBrush hSpacingBrush(palette.color(cg, DPalette::FrameBorder));
        // vertical spacing
        QBrush vSpacingBrush(QBrush(QColor("#1A000000")));
        QRectF vSpacingRect(rect.x(), rect.y() + kSpacingMargin, 1,
                            rect.height() - kSpacingMargin * 2);
        QBrush clearBrush(palette.color(cg, DPalette::Window));

        painter->fillRect(hSpacingRect, clearBrush);
        painter->fillRect(hSpacingRect, hSpacingBrush);

        if (visualIndex(logicalIndex) > 0) {
            painter->fillRect(vSpacingRect, clearBrush);
            painter->fillRect(vSpacingRect, vSpacingBrush);
        }

        QPen forground;
        forground.setColor(palette.color(cg, DPalette::Text));
        QRect textRect = {contentRect.x() + margin, contentRect.y(), contentRect.width() - margin,
                        contentRect.height()};
        QString title = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
        int align = Qt::AlignLeft | Qt::AlignVCenter;

        painter->setPen(forground);
        painter->drawText(textRect, static_cast<int>(align), title);
        painter->restore();
}

void BorderRadiusHeaderView::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.save();

    DPalette::ColorGroup cg = DPalette::Active;
    DPalette palette = DApplicationHelper::instance()->applicationPalette();

    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    if (!style)
        return;

    QBrush bgBrush(QColor(isDarkMode() ? "#CC202020" : "#FFFFFFFF"));

    QStyleOptionHeader option;
    initStyleOption(&option);
    int radius = 8;
    int thickness = 1;

    QRect rect = viewport()->rect();
    painter.fillRect(rect, bgBrush);

    // 画圆角边框
    // 绘制表格外边框采用填充方式,通过内矩形与外矩形相差得到填充区域
    QPainterPath paintPath, paintPathOut, paintPathIn;
    rect.setHeight(rect.height() + radius);
    // 外圆角矩形路径,即表格的外边框路径
    paintPathOut.addRoundedRect(rect, radius, radius);

    // 内圆角矩形路径,与外圆角矩形上下左右相差thickness个像素
    QRect rectIn = QRect(rect.x() + thickness, rect.y() + thickness, rect.width() - thickness * 2, rect.height() - thickness * 2);
    paintPathIn.addRoundedRect(rectIn, radius, radius);

    // 填充路径
    paintPath = paintPathOut.subtracted(paintPathIn);

    // 填充
    QBrush borderBrush(QColor("#1A000000"));
    painter.fillPath(paintPath, borderBrush);

    // 四角圆弧外围遮罩
    QPainterPath cornerOutPath;
    cornerOutPath.addRect(rect);
    cornerOutPath = cornerOutPath.subtracted(paintPathOut);
    painter.fillPath(cornerOutPath, QBrush(QColor(isDarkMode() ? "#232323" : "#FFFFFF")));

    painter.restore();
    DHeaderView::paintEvent(event);
}
