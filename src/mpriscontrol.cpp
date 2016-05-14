#include <stdio.h>
#include "mpriscontrol.h"

MprisControl::MprisControl(QObject *parent) :
    QObject(parent)
{
}

void MprisControl::init()
{
    mpris2Service = QString();

    QDBusConnection bus = QDBusConnection::sessionBus();

    serviceWatcher = new QDBusServiceWatcher(this);
    serviceWatcher->setConnection(bus);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &MprisControl::ownerChanged);
}

void MprisControl::sendMpris2(const QString &methodName)
{
    if (mpris2Service.isEmpty())
    {
        if (!getMpris2Service())
        {
            printf("buttonjackd: No mpris2 service available\n");
            return;
        }
    }

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall(mpris2Service,
                                                       "/org/mpris/MediaPlayer2",
                                                       "org.mpris.MediaPlayer2.Player",
                                                       methodName);

    QDBusError error = bus.call(call);

    if (error.isValid())
    {
        printf("buttonjackd: mpris2 %s failed: %s\n", qPrintable(mpris2Service), qPrintable(error.message()));
    }
}

bool MprisControl::getMpris2Service()
{
    bool ret = false;

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusConnectionInterface *bus_iface = bus.interface();

    const QStringList &services = bus_iface->registeredServiceNames();
    foreach (QString service, services)
    {
        if (service.startsWith("org.mpris.MediaPlayer2."))
        {
            mpris2Service = service;
            printf("buttonjackd: found mpris2 service: %s\n", qPrintable(mpris2Service));
            ret = true;
            break;
        }
    }
    return ret;
}

void MprisControl::ownerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);

    if (name == mpris2Service && newOwner.isEmpty())
    {
        printf("mpris2 service %s removed from bus\n", qPrintable(mpris2Service));
        mpris2Service = QString();
    }
}

