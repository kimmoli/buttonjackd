#include <stdio.h>
#include "volumecontrol.h"

VolumeControl::VolumeControl(QObject *parent) :
    QObject(parent)
{
    pulseAudioBus = NULL;
}

int VolumeControl::init()
{
    if (pulseAudioBus != NULL)
    {
        printf("buttonjackd: volumecontrol is already initialised\n");
        return EXIT_FAILURE;
    }

    QDBusConnection bus = QDBusConnection::sessionBus();

    QDBusMessage call = QDBusMessage::createMethodCall("org.PulseAudio1", "/org/pulseaudio/server_lookup1", "org.freedesktop.DBus.Properties", "Get" );
    call << "org.PulseAudio.ServerLookup1" << "Address";
    QDBusReply<QDBusVariant> lookupReply = bus.call(call);

    if (lookupReply.isValid())
    {
        pulseAudioBus = new QDBusConnection(QDBusConnection::connectToPeer(lookupReply.value().variant().toString(), "org.PulseAudio1"));
    }
    else
    {
        printf("buttonjackd: Can not find pulseaudio from dbus\n");
        return EXIT_FAILURE;
    }

    call = QDBusMessage::createMethodCall("com.Meego.MainVolume2", "/com/meego/mainvolume2", "org.freedesktop.DBus.Properties", "Get");
    call << "com.Meego.MainVolume2" << "StepCount";
    QDBusReply<QDBusVariant> volumeMaxReply = pulseAudioBus->call(call);

    if (volumeMaxReply.isValid())
    {
        maxVolume = volumeMaxReply.value().variant().toUInt();
    }
    else
    {
        printf("buttonjackd: Could not read volume max, cannot adjust volume: %s\n", qPrintable(volumeMaxReply.error().message()));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void VolumeControl::changeVolume(bool increase)
{
    if (pulseAudioBus == NULL)
    {
        printf("buttonjackd: volumecontrol not properly initialised\n");
        return;
    }

    QDBusMessage call = QDBusMessage::createMethodCall("com.Meego.MainVolume2", "/com/meego/mainvolume2", "org.freedesktop.DBus.Properties", "Get");
    call << "com.Meego.MainVolume2" << "CurrentStep";
    QDBusReply<QDBusVariant> volumeReply = pulseAudioBus->call(call);

    if (volumeReply.isValid())
    {
        uint volume = volumeReply.value().variant().toUInt();
        uint newVolume;

        if (increase && volume < maxVolume-1 )
        {
            newVolume = volume + 1;
        }
        else if (!increase && volume > 0)
        {
            newVolume = volume - 1;
        }
        else
        {
            newVolume = volume;
        }

        if (newVolume != volume)
        {
            printf("buttonjackd: Setting volume to %d\n", newVolume);

            call = QDBusMessage::createMethodCall("com.Meego.MainVolume2", "/com/meego/mainvolume2", "org.freedesktop.DBus.Properties", "Set");
            call << "com.Meego.MainVolume2" << "CurrentStep" << QVariant::fromValue(QDBusVariant(newVolume));

            QDBusError err = pulseAudioBus->call(call);

            if (err.isValid())
            {
                printf("buttonjackd: Volume change throw error %s\n", qPrintable(err.message()));
            }
        }
    }
}
