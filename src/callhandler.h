#ifndef CALLHANDLER_H
#define CALLHANDLER_H

#include <QObject>
#include <QDBusMessage>

class CallHandler : public QObject
{
    Q_OBJECT
public:
    explicit CallHandler(QObject *parent = 0);
    void init();

    typedef enum CallState
    {
        None = 0,
        Ringing,
        Active
    } CallState;

public slots:
    void callOperation(QString);

signals:
    void callStateChanged(CallHandler::CallState);

private slots:
    void handleCall(const QDBusMessage& msg);

};

#endif // CALLHANDLER_H
