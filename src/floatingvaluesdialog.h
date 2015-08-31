#ifndef FLOATINGVALUESDIALOG_H
#define FLOATINGVALUESDIALOG_H

#include <QDialog>

#include <QGridLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QToolButton>

#include <QDebug>

class FloatingValuesDialog : public QDialog
{

    Q_OBJECT

public:
    enum INPUTWIDGETS { VOLTAGE, CURRENT, TRACKING };

    FloatingValuesDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:

    void doubleValueAccepted(const double &val, const int &w);

public slots:

    void setWidget(const FloatingValuesDialog::INPUTWIDGETS &w);
    void setWidgetValue(const double &value);
    void setWidgetValue(const int &trackingMode);
    void updateDeviceSpecs(const double &voltageMin, const double &voltageMax,
                           const uint &voltagePrecision, const double &currentMin,
                           const double &currentMax, const uint &currentPrecision,
                           const uint &noOfChannels);

private:
    int channel;

    QGridLayout *mainLayout;
    QStackedWidget *stackedContainer;
    QToolButton *acceptButton;

    QDoubleSpinBox *voltageSpinBox;
    QDoubleSpinBox *currentSpinBox;

    void createUI();

private slots:
    void accept();
    void reject();
};

#endif // FLOATINGVALUESDIALOG_H
