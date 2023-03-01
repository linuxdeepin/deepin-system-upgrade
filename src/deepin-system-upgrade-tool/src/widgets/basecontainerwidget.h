// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <DPushButton>
#include <DSuggestButton>

#include <QWidget>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../widgets/threedotswidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief 主要功能界面的容器。负责放界面内容以及（自行）实现界面底部的三个按钮。
 *
 */
class BaseContainerWidget : public QWidget
{
public:
    explicit BaseContainerWidget(QWidget *parent = nullptr, int aDot = 0);
    void addContentWidget(QWidget *widget);

protected:
    DPushButton     *m_cancelButton;
    DPushButton     *m_midButton;
    DSuggestButton  *m_suggestButton;
    QSpacerItem     *m_spacerItem;
    QSpacerItem     *m_buttonSpacerItem;
    QVBoxLayout     *m_mainLayout;
    QVBoxLayout     *m_contentLayout;
    QVBoxLayout     *m_bottomUpperLayout;
    QVBoxLayout     *m_bottomLayout;
    QHBoxLayout     *m_buttonLayout;
    ThreeDotsWidget *m_threeDotsWidget;

    void initUI();
};
