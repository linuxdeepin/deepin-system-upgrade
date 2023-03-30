// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QVBoxLayout>
#include <QWidget>

#include "../../widgets/spinnerwidget.h"
#include "isoinfochecker.h"

class CheckImageWidget: public QWidget
{
    Q_OBJECT
public:
    CheckImageWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void Cancel();
    // Validate the imported system image
    void CheckImported(const QString path);
    // 本地导入检查的几个阶段完成信号
    void EditionNameAvailable(const QString editionName);
    void VersionAvailable(const QString version);
    void IntegrityStatusAvailable(int status);
    // Check the downloaded image. Delete the image if check fails.
    void CheckDownloaded();
    void CheckDone(bool state);

private Q_SLOTS:
    void onCanceled();
    void onCheckImported(const QString path);
    void onEditionNameAvailable(const QString editionName);
    void onVersionAvailable(const QString version);
    void onIntegrityStatusAvailable(int status);
    void onCheckDownloaded();

private:
    bool            m_canceled;
    QString         m_isoPath;
    QVBoxLayout     *m_mainLayout;
    SpinnerWidget   *m_spinnerWidget;
    IsoInfoChecker  *m_editionNameChecker;
    IsoInfoChecker  *m_versionChecker;
    IsoInfoChecker  *m_integrityChecker;

    void initUI();
    void initConnections();

};
