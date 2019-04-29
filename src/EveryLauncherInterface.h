/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp com.gitee.wanywhn.EveryLauncher.xml -p EveryLauncherInterface -c EveryLauncherInterface
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef EVERYLAUNCHERINTERFACE_H
#define EVERYLAUNCHERINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.gitee.wanywhn.EveryLauncher
 */
class EveryLauncherInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.gitee.wanywhn.EveryLauncher"; }

public:
    EveryLauncherInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~EveryLauncherInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> IndexChangeFiles(const QStringList &paths)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(paths);
        return asyncCallWithArgumentList(QStringLiteral("IndexChangeFiles"), argumentList);
    }

    inline QDBusPendingReply<> showWindow()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("showWindow"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace gitee {
    namespace wanywhn {
      typedef ::EveryLauncherInterface EveryLauncher;
    }
  }
}
#endif
