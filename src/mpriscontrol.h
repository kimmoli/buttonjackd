#ifndef MPRISCONTROL_H
#define MPRISCONTROL_H

#include <QObject>
#include <QtDBus/QtDBus>

class MprisControl : public QObject
{
    Q_OBJECT
public:
    explicit MprisControl(QObject *parent = 0);
    void init();

public slots:
    void sendMpris2(const QString &methodName);

private slots:
    void ownerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

private:
    bool getMpris2Service();
    QString mpris2Service;
    QDBusServiceWatcher *serviceWatcher;
};

#endif // MPRISCONTROL_H
