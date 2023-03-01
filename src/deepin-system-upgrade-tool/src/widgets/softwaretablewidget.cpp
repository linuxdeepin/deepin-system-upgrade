// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DApplicationHelper>

#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>

#include "../core/constants.h"
#include "../core/utils.h"
#include "softwaretablewidget.h"
#include "../tool/application.h"

void SoftwareTableWidget::initUI()
{
    this->setColumnCount(2);

    // 设置不可聚焦
    setFocusPolicy(Qt::NoFocus);

    // 设置无边框
    this->setFrameStyle(QFrame::NoFrame);

    // 设置背景
    setAutoFillBackground(true);

    // 设置不可编辑模式
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置隐藏表格滚动条
//    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    this->verticalScrollBar()->setDisabled(true);

    // 设置不可选择
    this->setSelectionMode(QAbstractItemView::NoSelection);

    // 不显示网格线
    this->setShowGrid(false);

    // 设置各行变色
    setAlternatingRowColors(true);
    setRowColorStyles();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, [this] {
        setRowColorStyles();
    });

    // 隐藏表头
    this->verticalHeader()->setVisible(false);
    setHorizontalHeader(m_headerView);
    this->horizontalHeader()->setVisible(false);

    // 设置不可排序
    this->setSortingEnabled(false);

    // 设置填充
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setDefaultSectionSize(SOFTWARE_TABLE_ROW_H);
    horizontalHeader()->stretchLastSection();

    // 设置行高
    this->verticalHeader()->setDefaultSectionSize(SOFTWARE_TABLE_ROW_H);

    this->setAttribute(Qt::WA_TranslucentBackground);   // 设置窗口背景透明
    this->setWindowFlags(Qt::FramelessWindowHint);      // 设置无边框窗口
}

void SoftwareTableWidget::paintEvent(QPaintEvent *event)
{
    QWidget *wnd = Application::getInstance()->activeWindow();
    DPalette::ColorGroup cg;
    if (!wnd) {
        cg = DPalette::Inactive;
    } else {
        cg = DPalette::Active;
    }

    QRect rect = this->rect();
    rect.setY(rect.y() - m_headerView->height());
    rect.setHeight(rect.height() - m_headerView->height());
    QPainter painter(this->viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setOpacity(1);
    painter.setClipping(true);

    DPalette palette = DApplicationHelper::instance()->applicationPalette();

    // 绘制表格外边框采用填充方式,通过内矩形与外矩形相差得到填充区域
    int width = 1;
    int radius = 8;
    QPainterPath paintPath, paintPathOut, paintPathIn;

    // 外圆角矩形路径,即表格的外边框路径
    paintPathOut.addRoundedRect(rect, radius, radius);

    // 内圆角矩形路径,与外圆角矩形上下左右相差1个像素
    QRect rectIn = QRect(rect.x() + width, rect.y() + width, rect.width() - width * 2, rect.height() - width * 2);
    paintPathIn.addRoundedRect(rectIn, radius, radius);

    // 填充路径
    paintPath = paintPathOut.subtracted(paintPathIn);

    // 填充
    QBrush bgBrush(QColor("#1A000000"));
    painter.fillPath(paintPath, bgBrush);

    // 画中间分割线
    painter.fillRect(rect.x() + rect.width() / 2, rect.y(), width, rect.height(), bgBrush);

    // 四角圆弧外围遮罩
    QPainterPath cornerOutPath;
    cornerOutPath.addRect(rect);
    cornerOutPath = cornerOutPath.subtracted(paintPathOut);
    painter.fillPath(cornerOutPath, QBrush(QColor(isDarkMode() ? "#232323" : "#FFFFFF")));

    painter.restore();
    DTableWidget::paintEvent(event);
}

void SoftwareTableWidget::wheelEvent(QWheelEvent *event)
{
    DTableWidget::wheelEvent(event);
    update();
}

void SoftwareTableWidget::setRowColorStyles()
{
    QPalette palette = this->palette();
    if (isDarkMode())
    {
        palette.setColor(QPalette::Base, QColor("#0DFFFFFF"));
        palette.setColor(QPalette::AlternateBase, QColor("#FF232323"));
    }
    else
    {
        palette.setColor(QPalette::Base, QColor("#FFFFFFFF"));
        palette.setColor(QPalette::AlternateBase, QColor("#08000000"));
    }
    setPalette(palette);
}
