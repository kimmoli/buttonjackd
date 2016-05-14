#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

class VolumeControl : public QObject
{
    Q_OBJECT
public:
    explicit VolumeControl(QObject *parent = 0);
    int init();

signals:

public slots:
    void changeVolume(bool increase);

private:
    QDBusConnection *pulseAudioBus;
    uint maxVolume;

};

#endif // VOLUMECONTROL_H
