#include "cutrect.h"

CutRect::CutRect()
{

}

CutRect::CutRect(QRectF rect) : rect(rect.x(), rect.y(), rect.width(), rect.height())
{

}

void CutRect::setText(QString str)
{
    this->str = str;
}

void CutRect::drawMe(QPainter &painter)
{
    painter.save();
    painter.setPen(QPen());
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.drawRect(rect);
    //设置白色文字
    painter.setPen(QColor(255, 255, 255));
    //居中画出文字
    painter.drawText(rect, Qt::AlignCenter, str);
    painter.restore();
}

void CutRect::setLocation(qreal x, qreal y)
{
    rect.setX(x);               //设置方框的起点坐标，位于传入坐标的上方距离21的点
    rect.setY(y - 21);
    rect.setWidth(71);          //设置方框的长宽
    rect.setHeight(21);
}
