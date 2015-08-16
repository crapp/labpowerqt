#ifndef LABPOWERCONTROLLER_H
#define LABPOWERCONTROLLER_H

#include <QObject>

class LabPowerController : public QObject
{
    Q_OBJECT
public:
    explicit LabPowerController(QObject *parent = 0);

signals:

public slots:
};

#endif // LABPOWERCONTROLLER_H
