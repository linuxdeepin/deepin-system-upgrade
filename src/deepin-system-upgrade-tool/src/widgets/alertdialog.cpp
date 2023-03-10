// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "alertdialog.h"
#include "../tool/mainwindow.h"

AlertDialog::AlertDialog(QWidget *parent, const QString title, const QString acceptText, DDialog::ButtonType type)
    : DDialog(parent)
{
    initUI(title, acceptText, type);
    MainWindow *window = MainWindow::getInstance();
    move(window->x() + (window->rect().width() - width()) / 2, window->y() + (window->rect().height() - height()) / 2);
}

void AlertDialog::initUI(const QString title, const QString acceptText, DDialog::ButtonType type)
{
    setTitle(title);
    setIcon(QIcon::fromTheme("dialog-warning"));
    addButton(tr("Cancel"));
    addButton(acceptText, false, type);
}
