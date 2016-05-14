#include <stdio.h>
#include "callhandler.h"
#include <QDBusConnection>

CallHandler::CallHandler(QObject *parent) :
    QObject(parent)
{
}

void CallHandler::init()
{
    static QDBusConnection mceCallStateconn = QDBusConnection::systemBus();
    mceCallStateconn.connect("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal", "sig_call_state_ind",
                          this, SLOT(handleCall(const QDBusMessage&)));

    if (mceCallStateconn.isConnected())
    {
        printf("com.nokia.mce.signal.sig_call_state_ind Connected\n");
    }
    else
    {
        printf("com.nokia.mce.signal.sig_call_state_ind Not connected\n%s\n", qPrintable(mceCallStateconn.lastError().message()));
    }
}

void CallHandler::handleCall(const QDBusMessage& msg)
{
    QList<QVariant> args = msg.arguments();

    printf("buttonjackd: Call state: %s\n", qPrintable(args.at(0).toString()));

    if (!(QString::localeAwareCompare( args.at(0).toString(), "ringing")))
    {
        emit callStateChanged(Ringing);
    }
    else if (!(QString::localeAwareCompare( args.at(0).toString(), "active")))
    {
        emit callStateChanged(Active);
    }
    else
    {
        emit callStateChanged(None);
    }
}

void CallHandler::callOperation(QString method)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("org.nemomobile.voicecall",
                                                       "/calls/active",
                                                       "org.nemomobile.voicecall.VoiceCall",
                                                       method);

    QDBusError error = bus.call(call);

    if (error.isValid())
    {
        printf("buttonjackd: %s failed %s\n", qPrintable(method), qPrintable(error.message()));
    }
}
