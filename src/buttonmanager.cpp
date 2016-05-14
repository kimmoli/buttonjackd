#include <stdio.h>
#include <linux/input.h>
#include "buttonmanager.h"

ButtonManager::ButtonManager(QObject *parent) :
    QObject(parent)
{
    longPress = new QTimer();
    longPress->setSingleShot(true);
    longPress->setInterval(1000);
    pressedKey = 0;

    connect(longPress, SIGNAL(timeout()), this, SLOT(longPressExpired()));
}

/* BTN_1 and BTN_2
 *
 * short presses controls volume on release
 * long presses mpris next and previous track
 * Note that this requires modified kernel, which has EV_KEY BTN_1 and BTN_2 instead of KEY_VOLUMEUP/DOWN
 *
 * KEY_MEDIA
 *
 * mpris play/pause
 *
 */

void ButtonManager::buttonStateChanged(int keycode, bool down)
{
    printf("buttonjackd: key %d %s (current %d)\n", keycode, down ? "pressed" : "released", pressedKey);

    if (down)
    {
        pressedKey = keycode;
        longPress->start();
    }
    else
    {
        longPress->stop();

        if (pressedKey == KEY_MEDIA)
        {
            emit sendMpris2("PlayPause");
        }
        else if (pressedKey == BTN_1)
        {
            emit changeVolume(true);
        }
        else if (pressedKey == BTN_2)
        {
            emit changeVolume(false);
        }
    }
}

void ButtonManager::longPressExpired()
{
    printf("buttonjackd: key %d pressed long\n", pressedKey);

    if (pressedKey == BTN_1)
    {
        emit sendMpris2("Next");
        pressedKey = 0;
    }
    else if (pressedKey == BTN_2)
    {
        emit sendMpris2("Previous");
        pressedKey = 0;
    }
}