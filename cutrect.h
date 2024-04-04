#ifndef CUTRECT_H
#define CUTRECT_H

#include <QObject>
#include <QPainter>

class CutRect : public QObject
{
    Q_OBJECT
public:
    CutRect();
    CutRect(QRectF rect);
    void setText(QString str);                  //设置文字（框选区尺寸）
    void drawMe(QPainter &painter);             //绘制自身
    void setLocation(qreal x, qreal y);

private:
    QRectF rect;
    QString str;
};

#endif // CUTRECT_H
