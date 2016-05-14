#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <QObject>
#include <QTimer>

class ButtonManager : public QObject
{
    Q_OBJECT
public:
    explicit ButtonManager(QObject *parent = 0);

signals:
    void changeVolume(bool increase);
    void sendMpris2(QString);

public slots:
    void buttonStateChanged(int keycode, bool down);

private slots:
    void longPressExpired();

private:
    QTimer *longPress;
    int pressedKey;

};

#endif // BUTTONMANAGER_H
