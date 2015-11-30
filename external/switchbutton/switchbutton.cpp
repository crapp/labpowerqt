#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include "switchbutton.h"

SwitchButton::SwitchButton(QWidget* parent) : QPushButton(parent) {
    setCheckable(true);
    // Set default colors and labels
    setColors();
    setLabels();
}

void SwitchButton::setColors(const QColor on, const QColor off) {
    onColor=on;
    offColor=off;
    if (on.red()+on.green()+on.blue()>500) {
        onLabelColor=Qt::black;
    }
    else {
        onLabelColor=Qt::white;
    }
    if (off.red()+off.green()+off.blue()>500) {
        offLabelColor=Qt::black;
    }
    else {
        offLabelColor=Qt::white;
    }
}

void SwitchButton::setLabels(const QString on, const QString off) {
    onLabel=on;
    offLabel=off;
    setMinimumWidth(fontMetrics().width(offLabel)+fontMetrics().width(onLabel)+fontMetrics().height()*2);
}

void SwitchButton::paintEvent(QPaintEvent* paint) {
    QPushButton::paintEvent(paint);
    QPainter p(this);
    p.save();
    int rectWidth=paint->rect().width()/2;
    #ifdef Q_OS_ANDROID
        // On Android, the buttons are much smaller than paint->rect().
        int rectMargin=10;
    #else
        int rectMargin=4;
    #endif
    if (isChecked()) {
        QRect rect=paint->rect().adjusted(rectWidth,rectMargin,-rectMargin,-rectMargin);
        p.fillRect(rect,QBrush(onColor));
        QPoint textPosi=rect.center()-QPoint(fontMetrics().width(onLabel)/2,1-fontMetrics().ascent()/2);
        p.setPen(onLabelColor);
        p.drawText(textPosi,onLabel);
    }
    else {
        QRect rect=paint->rect().adjusted(rectMargin,rectMargin,-rectWidth,-rectMargin);
        p.fillRect(rect,QBrush(offColor));
        QPoint textPosi=rect.center()-QPoint(fontMetrics().width(offLabel)/2,1-fontMetrics().ascent()/2);
        p.setPen(offLabelColor);
        p.drawText(textPosi,offLabel);
    }
    p.restore();
}
