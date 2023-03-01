// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <DGuiApplicationHelper>

#include <QDir>
#include <QFile>

#include "../../core/constants.h"
#include "../../core/dbusworker.h"
#include "../../core/utils.h"
#include "softwareevaluationwidget.h"
#include "../../widgets/iconlabel.h"

AppInfo readDesktopFile(const QString &filePath)
{
    QFile desktopFile(filePath);
    AppInfo info;
    info.noDisplay = false;
    QString name = "";
    QString fallbackName = "";
    if (desktopFile.open(QFile::ReadOnly | QFile::Text))
    {
        while (!desktopFile.atEnd())
        {
            QString line = desktopFile.readLine();
            if (line.startsWith("Icon"))
            {
                info.iconName = line.split("=")[1].trimmed();
            }
            else if (line.startsWith(QString("Name[%1]").arg(kCurrentLocale)))
            {
                name = line.split("=")[1].trimmed();
            }
            else if (line.startsWith("Name="))
            {
                fallbackName = line.split("=")[1].trimmed();
            }
            else if (line.startsWith("NoDisplay="))
            {
                info.noDisplay = line.split("=")[1].trimmed() == "true";
            }
        }
        desktopFile.close();
        info.name = name.length() > 0 ? name : fallbackName;
    }
    else
    {
        info.name = filePath;
    }
    return info;
}

SoftwareEvaluationWidget::SoftwareEvaluationWidget(QWidget *parent)
    :   DWidget(parent)
    ,   m_pkgDesktopMap(new QMap<QString, QStringList>())
    ,   m_mainLayout(new QVBoxLayout(this))
    ,   m_mainWidget(new QWidget(this))
    ,   m_softwareChangeTitleLabel(new DLabel(this))
    ,   m_softwareChangeTableWidget(new SoftwareTableWidget(this))
    ,   m_powerTipLabel(new SimpleLabel)
{
    initUI();
    initConnections();
}

void SoftwareEvaluationWidget::initConnections()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, [this] {
        QPalette palette = this->palette();
        if (isDarkMode())
        {
            palette.setColor(QPalette::Base, "#FF232323");
        }
        else
        {
            palette.setColor(QPalette::Base, "#FFFFFFFF");
        }
        this->setPalette(palette);
    });
    connect(DBusWorker::getInstance(), &DBusWorker::AppsAvailable, [this] (const QMap<QString, QStringList> appSet){
        *m_pkgDesktopMap = appSet;
        qDebug() << "m_pkgDesktopMap:" << *m_pkgDesktopMap;
        qDebug() << "m_pkgDesktopMap size:" << m_pkgDesktopMap->size();
    });
    connect(DBusWorker::getInstance(), &DBusWorker::MigrateStatus, this, &SoftwareEvaluationWidget::updateAppInfos);
}

void SoftwareEvaluationWidget::initUI()
{
    QPalette palette = this->palette();
    QFont titleFont("SourceHanSansSC", 12, QFont::Bold);
    QFont headerFont("SourceHanSansSC", 9, QFont::Medium);

    if (isDarkMode())
    {
        palette.setColor(QPalette::Base, "#FF232323");
    }
    else
    {
        palette.setColor(QPalette::Base, "#FFFFFFFF");
    }
    this->setPalette(palette);

//    setWidgetResizable(true);

    m_softwareChangeTitleLabel->setText(tr("Evaluation Result"));
    m_softwareChangeTitleLabel->setForegroundRole(DPalette::TextTitle);
    m_softwareChangeTitleLabel->setFont(titleFont);

    const QStringList softwareChangeHeaders = {tr("Compatible Apps"), tr("Incompatible Apps")};
    m_softwareChangeTableWidget->setHorizontalHeaderLabels(softwareChangeHeaders);
    m_softwareChangeTableWidget->horizontalHeader()->setFont(headerFont);
    m_softwareChangeTableWidget->horizontalHeader()->setForegroundRole(QPalette::Text);
    m_softwareChangeTableWidget->horizontalHeader()->setVisible(true);
    m_softwareChangeTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_softwareChangeTableWidget->horizontalHeader()->setFixedHeight(36);

    m_powerTipLabel->setFont(QFont("SourceHanSansSC", 9, QFont::Normal));
    m_powerTipLabel->setForegroundRole(DPalette::TextTips);
    m_powerTipLabel->setText(tr("The upgrade may take a long time. Please plug in to avoid interruption."));

    m_mainLayout->setContentsMargins(74, 0, 74, 0);
    m_mainLayout->addSpacing(30);
    m_mainLayout->addWidget(m_softwareChangeTitleLabel, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_softwareChangeTableWidget);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_powerTipLabel, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(8);
    setLayout(m_mainLayout);

    // 必须设置成和Base表格项一样的Role，否则会导致Base表格项背景异常显示底层的控件。
    setBackgroundRole(QPalette::Base);

//    setWidget(m_mainWidget);

//    setFrameStyle(QFrame::NoFrame);
}

IconLabel* generateCellWidget(const QString name, const QString iconPath)
{
    const QFont titleFont("SourceHanSansSC", 9, QFont::Medium);
    IconLabel *cellLabel = new IconLabel(nullptr);
    DLabel *nameLabel = new DLabel(name);
    nameLabel->setFont(titleFont);
    nameLabel->setForegroundRole(DPalette::TextTitle);
    // Icon用的是绝对路径文件
    if (iconPath.startsWith('/'))
    {
        cellLabel->setIcon(QIcon(iconPath).pixmap(24, 24));
    }
    else
    {
        cellLabel->setIcon(QIcon::fromTheme(iconPath, QIcon::fromTheme("application-x-executable")).pixmap(24, 24));
    }
    cellLabel->addSpacing(10);
    cellLabel->addWidget(nameLabel);
    cellLabel->addSpacing(114514);
    return cellLabel;
}

void SoftwareEvaluationWidget::fillTable()
{
    m_softwareChangeTableWidget->setRowCount(m_compatAppInfos.size() > m_incompatAppInfos.size() ? m_compatAppInfos.size() : m_incompatAppInfos.size());
//    m_softwareChangeTableWidget->setFixedHeight(SOFTWARE_TABLE_ROW_H * m_softwareChangeTableWidget->rowCount() + 36);
    for (unsigned long i = 0; i < m_compatAppInfos.size(); ++i) {
        AppInfo info = m_compatAppInfos[i];
        IconLabel *cellLabel = generateCellWidget(info.name, info.iconName);
        m_softwareChangeTableWidget->setCellWidget(i, 0, cellLabel);
        qDebug() << "compat app:" << info.name;
    }
    for (unsigned long i = 0; i < m_incompatAppInfos.size(); ++i) {
        AppInfo info = m_incompatAppInfos[i];
        IconLabel *cellLabel = generateCellWidget(info.name, info.iconName);
        m_softwareChangeTableWidget->setCellWidget(i, 1, cellLabel);
        qDebug() << "incompat app:" << info.name;
    }
}

void SoftwareEvaluationWidget::clearAppInfos()
{
    m_compatAppInfos.clear();
    m_incompatAppInfos.clear();
}

void SoftwareEvaluationWidget::updateAppInfos(const QString pkg, int status)
{
    QDir usrShareDir = QDir("/usr/share");
    QDir iconDir = QDir(usrShareDir);
    iconDir.cd("icons");
    QDir appDir = QDir(usrShareDir);
    appDir.cd("applications");

    QIcon::setThemeSearchPaths(QStringList(iconDir.absolutePath()));
    if (m_pkgDesktopMap->contains(pkg))
    {
        for (QString desktopFilename: m_pkgDesktopMap->value(pkg))
        {
            AppInfo info = readDesktopFile(appDir.absoluteFilePath(desktopFilename));
            qDebug() << "Adding desktop entry" << desktopFilename << "for" << pkg;
            if (status == 1)
                m_compatAppInfos.push_back(info);
            else
                m_incompatAppInfos.push_back(info);
        }
    }
}
