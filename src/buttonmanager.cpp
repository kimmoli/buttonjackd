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

    callState = CallHandler::None;
}

/* BTN_0
 *
 * mpris play/pause
 * answer incoming call or hangup active call
 *
 * BTN_1 and BTN_2
 *
 * short presses controls volume on release
 * long presses mpris next and previous track
 * Note that this requires modified kernel, which has EV_KEY BTN_1 and BTN_2 instead of KEY_VOLUMEUP/DOWN
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

        if (pressedKey == BTN_0 || pressedKey == KEY_MEDIA)
        {
            if (callState == CallHandler::None)
                emit sendMpris2("PlayPause");
            else if (callState == CallHandler::Ringing)
                emit callOperation("answer");
            else if (callState == CallHandler::Active)
                emit callOperation("hangup");
        }
        else if (pressedKey == BTN_2 || pressedKey == BTN_3)
        {
            /* Volume up */
            emit changeVolume(true);
        }
        else if (pressedKey == BTN_1 || pressedKey == BTN_7)
        {
            /* Volume down */
            emit changeVolume(false);
        }
    }
}

void ButtonManager::longPressExpired()
{
    printf("buttonjackd: key %d pressed long\n", pressedKey);

    if (pressedKey == BTN_2 || pressedKey == BTN_3)
    {
        /* Next track */
        emit sendMpris2("Next");
        pressedKey = 0;
    }
    else if (pressedKey == BTN_1 || pressedKey == BTN_7)
    {
        /* Previous track */
        emit sendMpris2("Previous");
        pressedKey = 0;
    }
}

void ButtonManager::callStateChanged(CallHandler::CallState newCallState)
{
    callState = newCallState;
}
