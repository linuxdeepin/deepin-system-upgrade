// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <DLabel>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <DPushButton>
#include <QStackedWidget>
#include <QSpacerItem>

#include "../../widgets/basecontainerwidget.h"
#include "../../widgets/dropframe.h"
#include "../../widgets/baselabel.h"
#include "../../widgets/iconlabel.h"

DWIDGET_USE_NAMESPACE

class RetrieveImageMethodWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RetrieveImageMethodWidget(QWidget *parent);
    void initUI();
    void initConnections();
    bool isLocalFileSelected() {
        return m_localRadioButton->isChecked();
    }
    QString getFileUrl() {
        return m_fileUrl;
    }

Q_SIGNALS:
    void FileAdded();
    void FileClear();
    void ToggleLocalButton(bool checked);

private:
    QString         m_fileUrl;
    QVBoxLayout     *m_verticalLayout;
    QRadioButton    *m_networkRadioButton;
    QRadioButton    *m_localRadioButton;
    DPushButton     *m_networkPushButton;
    DPushButton     *m_localPushButton;
    QHBoxLayout     *m_buttonsHBoxLayout;
    DLabel          *m_label;
    IconLabel       *m_dropAreaIconLabel;
    DropFrame       *m_dropAreaFrame;
    BaseLabel       *m_fileTextLabel;
    QLabel          *m_uploadTextLink;
    DLabel          *m_filenameLabel;
    QSpacerItem     *m_fileLabelSpacerItem;
    DPushButton     *m_removeFileButton;
    QString         m_darkBtnStylesheet;
    QString         m_lightBtnStylesheet;

    QString loadStyleSheet(const QString &styleFile);

private Q_SLOTS:
    void onFileSelected(const QString &filename);
    void onFileCleared();
    void setDefaultDropFrameStyle();
};
