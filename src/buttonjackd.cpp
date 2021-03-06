/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 * buttonjackd main
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <QtCore/QCoreApplication>
#include <QObject>
#include <QThread>
#include "uinputevpoll.h"
#include "buttonmanager.h"
#include "volumecontrol.h"
#include "mpriscontrol.h"
#include "callhandler.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("buttonjackd: starting daemon version %s\n", APPVERSION);

    ButtonManager *bm;
    UinputEvPoll *uin;
    VolumeControl *volcon;
    MprisControl *mpriscon;
    CallHandler *callhandler;
    QThread *uinThread;

    uin = new UinputEvPoll();
    bm = new ButtonManager();
    volcon = new VolumeControl();
    mpriscon = new MprisControl();
    callhandler = new CallHandler();
    uinThread = new QThread();

    QString dev;

    if (argc > 1)
        if (QString(argv[1]).startsWith("event"))
            dev = QString("/dev/input/%1").arg(argv[1]);

    if (dev.isEmpty())
    {
        printf("buttonjackd: missing or incorrect argument. give event# where # is a number of evdev for button jack.\n");
        return EXIT_FAILURE;
    }

    int fd = uin->openDevice(dev.toLocal8Bit().constData());
    if (fd == -1)
    {
        printf("buttonjackd: error opening input device %s\n", qPrintable(dev));
        return EXIT_FAILURE;
    }

    if (volcon->init() == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    mpriscon->init();
    callhandler->init();

    uin->moveToThread(uinThread);

    QObject::connect(uin, SIGNAL(buttonStateChanged(int,bool)), bm, SLOT(buttonStateChanged(int,bool)));
    QObject::connect(uin, SIGNAL(pollingRequested()), uinThread, SLOT(start()));
    QObject::connect(uinThread, SIGNAL(started()), uin, SLOT(doPoll()));
    QObject::connect(uin, SIGNAL(finished()), uinThread, SLOT(quit()), Qt::DirectConnection);
    QObject::connect(bm, SIGNAL(changeVolume(bool)), volcon, SLOT(changeVolume(bool)));
    QObject::connect(bm, SIGNAL(sendMpris2(QString)), mpriscon, SLOT(sendMpris2(QString)));
    QObject::connect(bm, SIGNAL(callOperation(QString)), callhandler, SLOT(callOperation(QString)));
    QObject::connect(callhandler, SIGNAL(callStateChanged(CallHandler::CallState)), bm, SLOT(callStateChanged(CallHandler::CallState)));

    uin->requestPolling(fd);

    return app.exec();
}
