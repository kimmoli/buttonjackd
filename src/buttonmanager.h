#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <QObject>
#include <QTimer>
#include "callhandler.h"

class ButtonManager : public QObject
{
    Q_OBJECT
public:
    explicit ButtonManager(QObject *parent = 0);

signals:
    void changeVolume(bool increase);
    void sendMpris2(QString);
    void callOperation(QString);

public slots:
    void buttonStateChanged(int keycode, bool down);
    void callStateChanged(CallHandler::CallState newCallState);

private slots:
    void longPressExpired();

private:
    QTimer *longPress;
    int pressedKey;
    CallHandler::CallState callState;

};

#endif // BUTTONMANAGER_H
