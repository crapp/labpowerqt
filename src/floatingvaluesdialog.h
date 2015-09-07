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
    FloatingValuesDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:

    void doubleValueAccepted(const double &val, const int &sourceWidget,
                             const int &sourceChannel);

public slots:

    void setSourceWidget(const int &sourcew);
    void setSourceChannel(const int &channel);
    void setInputWidget(const int &w);
    void setInputWidgetValue(const double &value);
    void setInputWidgetValue(const int &trackingMode);
    void updateDeviceSpecs(const double &voltageMin, const double &voltageMax,
                           const uint &voltagePrecision,
                           const double &currentMin, const double &currentMax,
                           const uint &currentPrecision,
                           const uint &noOfChannels);

private:
    QGridLayout *mainLayout;
    QStackedWidget *stackedContainer;
    QToolButton *acceptButton;

    QDoubleSpinBox *voltageSpinBox;
    QDoubleSpinBox *currentSpinBox;

    int sourceWidget;
    int sourceChannel;

    void createUI();

private slots:
    void accept();
    void reject();
};

#endif // FLOATINGVALUESDIALOG_H
