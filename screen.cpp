#include "screen.h"

#include <QScreen>
#include <QPainter>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QTime>
#include <QFileDialog>
#include <QThread>

Screen::Screen(QWidget *parent)
    : QWidget(parent)
    , widthInfoRect(QRectF(0, 0, 0, 0))
    , editorBar(NULL)

{
    //showMinimized();
    editorLabel = new ImageLabel(this);
    editorBar = new EditorDialog(this);
    connect(editorBar,&EditorDialog::editorButtonPressed,this,&Screen::handleEditorSignal);
    //Qt::WindowFlags nType = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
    //editorLabel->setWindowFlags(nType);
    setMouseTracking(true);
}

Screen::~Screen()
{
    if(editorBar != NULL)
    {
        delete editorBar;
    }
}

QPainterPath Screen::getCutPath()
{
    QPainterPath path;                                              //框选的路径
    path.moveTo(rect0.topLeft());
    path.lineTo(rect0.topRight());
    path.lineTo(rect0.bottomRight());
    path.lineTo(rect0.bottomLeft());
    path.lineTo(rect0.topLeft());
    return globalPath.subtracted(path);                             //抠掉框选区域后的路径
}

void Screen::drawCutAreaFourAngles(QPainter &painter)
{
    rect1.setX(rect0.topRight().x() - 3);                           // rect4┌────────────────────┐rect1
    rect1.setY(rect0.topRight().y() - 3);                           //      │                    │
    rect1.setWidth(6);                                              //      │      rect0         │
    rect1.setHeight(6);                                             //      │                    │
    rect2.setX(rect0.bottomRight().x() - 3);                        // rect3└────────────────────┘rect2
    rect2.setY(rect0.bottomRight().y() - 3);
    rect2.setWidth(6);
    rect2.setHeight(6);
    rect3.setX(rect0.bottomLeft().x() - 3);
    rect3.setY(rect0.bottomLeft().y() - 3);
    rect3.setWidth(6);
    rect3.setHeight(6);
    rect4.setX(rect0.x() - 3);
    rect4.setY(rect0.y() - 3);
    rect4.setWidth(6);
    rect4.setHeight(6);
    painter.save();                                                 //保存当前状态
    painter.setBrush(Qt::blue);                                     //设置画刷
    painter.drawRect(rect1);                                        //逐个绘制
    painter.drawRect(rect2);
    painter.drawRect(rect3);
    painter.drawRect(rect4);
    painter.restore();
}

int Screen::comparePoint(QPointF &p1, QPointF &p2)
{
    if(p1.x() < p2.x() && p1.y() < p2.y())      //左上到右下↘
    {                                                               //4┌────────────────────┐1
        return 2;                                                   // │                    │
    }                                                               //3└────────────────────┘2
    else if(p1.x() > p2.x() && p1.y() < p2.y()) //右上到左下↙
    {
        return 3;
    }
    else if(p1.x() < p2.x() && p1.y() > p2.y()) //左下到右上↗
    {
        return 1;
    }
    return 4;                                   //右下到左上↖
}

bool Screen::pointInRect(QPoint &p, QRectF &r)
{
    if( p.x() > r.x() && p.x() < r.x() + r.width() &&       //大于点4坐标，小于点2坐标
        p.y() > r.y() && p.y() < r.y() + r.height())
    {
        return true;
    }
    return false;
}

LINE Screen::pointNearLine(QPoint &p, QRectF &r)
{
    if(((p.x() < r.left())&&(p.x()>r.left()-5))&&((p.y()>r.y())&&(p.y()<r.bottom())))               //左边界
    {
        return LEFTLINE;
    }
    else if(((p.x() > r.right())&&(p.x()<r.right()+5))&&((p.y()>r.y())&&(p.y()<r.bottom())))        //右边界
    {
        return RIGHTLINE;
    }
    else if(((p.x()>r.x())&&(p.x()<r.right())) && ((p.y()>r.y()-5) && (p.y()<r.y())))               //上边界
    {
        return TOPLINE;
    }
    else if(((p.x()>r.x())&&(p.x()<r.right())) && ((p.y()>r.bottom()) && (p.y()<r.bottom()+5)))     //下边界
    {
        return BOTTOMLINE;
    }
    return NO;
}

RECTAREA Screen::pointInWhere(QPoint p)
{
    if(pointInRect(p,rect0))
    {
        return RECT0;
    }
    else if(pointInRect(p,rect1))
    {
        return RECT1;
    }
    else if(pointInRect(p,rect2))
    {
        return RECT2;
    }
    else if(pointInRect(p,rect3))
    {
        return RECT3;
    }
    else if(pointInRect(p,rect4))
    {
        return RECT4;
    }
    else
    {
        if(pointNearLine(p,rect0) == TOPLINE)
        {
            return  TOP;
        }
        else if(pointNearLine(p,rect0) == LEFTLINE)
        {
            return  LEFT;
        }
        else if(pointNearLine(p,rect0) == BOTTOMLINE)
        {
            return  BOTTOM;
        }
        else if(pointNearLine(p,rect0) == RIGHTLINE)
        {
            return  RIGHT;
        }
    }
    return OUTOFRECT;
}

//按照框选区域的位置和尺寸裁剪整张屏幕截图（！！！x y w h四个值都要乘以系统显示设置的缩放比）
QPixmap Screen::cutGrabPixmap()
{
    return pixmap.copy(graphRatio*rect0.x(),graphRatio*rect0.y(),graphRatio*rect0.width(),graphRatio*rect0.height());
}

//设置ImageLabel
void Screen::setImageLabel()
{
    editorLabel->setGeometry(rect0.x(),rect0.y(),rect0.width(),rect0.height());     //设置位置和尺寸，即框选区域
    editorLabel->setPixmap(cutGrabPixmap());                                        //将框选区域的像素图传入
    editorLabel->loadedImage(true);                                                 //设置已传入图像标志
    editorLabel->show();                                                            //显示

}

//将图片复制到系统剪切板
void Screen::copyToClipboard()
{
    QClipboard *board = QApplication::clipboard();
    board->setImage(editorLabel->sendImage());                  //从ImageLabel传回来的
    quit();
}

//将图片保存
void Screen::savePixmap()
{
    QString picName = QString("屏幕截图");
    QTime time;
    time = QTime::currentTime();                                //获取当前系统时间，用做伪随机数的种子
    srand(time.msec() + time.second() * 1000);
    QString randStr;
    randStr.setNum(rand());                                     //随机字符串
    picName.append(randStr);
    QString filename=QFileDialog::getSaveFileName(this,QStringLiteral("保存截图"),picName,"JPEG Files(*.jpg)");
    if(filename.length()>0){
        QImage pimage = editorLabel->sendImage();
        if(pimage.save(filename, "jpg"))
        {
            quit();
        }
    }
}

void Screen::quit()
{
    this->close();
}

//绘图事件
void Screen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)                             //忽略未引用event参数的警告
    QPainter paint(this);                       //创建一个绘图类
    paint.drawPixmap(0, 0, pixmap);             //在指定位置（0,0）绘制pixmap，即捕获到的全屏像素图

    paint.setPen(Qt::blue);                     //初始化画笔（蓝色）
    paint.setBrush(QColor(0, 0, 0, 100));       //初始化画刷（半透明黑色）
    QPainterPath path = getCutPath();           //设置路径（抠掉框选区域后剩下的区域，如果还没有框选就是整个屏幕）
    paint.drawPath(path);                       //将区域绘制出来（即设置好的蓝色边线、半透明黑色填充）
    widthInfoRect.drawMe(paint);                //绘制显示框选区尺寸的方框
    drawCutAreaFourAngles(paint);               //绘制框选区四角的小方块
}

//鼠标按下事件，“框选截图区”和“调整截图区”触发
void Screen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        pressedPoint = event->pos();
        if(!draggable)                          //未框选时启用拖拽框选
        {
            regionSelectable = true;
        }
        else                                    //否则表示已经框选过，则启用调整截图区
        {
            isDragging = true;
        }
    }
}

//鼠标按键松开事件，“框选完成时”和“拖动调整完成时”触发
void Screen::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    regionSelectable = false;                   //拖动完成后禁用可框选、可调整，并设置已框选标志
    isDragging = false;
    draggable = true;
    editorBar->setGeometry(rect0.x()+rect0.width()-352,rect0.y()+rect0.height(),350,25);
    editorBar->show();
}

//鼠标移动事件，“框选时”和“拖动调整时”触发，仅移动鼠标时忽略
void Screen::mouseMoveEvent(QMouseEvent *event)
{
    if(regionSelectable)                                      //框选区域可用时执行框选过程的绘图，并记录框选后的区域
    {
        movePoint = event->pos();                             //当前鼠标位置
        switch (comparePoint(pressedPoint, movePoint)) {      //判断框选时的拖动方向
        case 1://↗
            rect0.setBottomLeft(pressedPoint);
            rect0.setTopRight(movePoint);
            break;
        case 2://↘
            rect0.setTopLeft(pressedPoint);
            rect0.setBottomRight(movePoint);
            break;
        case 3://↙
            rect0.setTopRight(pressedPoint);
            rect0.setBottomLeft(movePoint);
            break;
        case 4://↖
            rect0.setBottomRight(pressedPoint);
            rect0.setTopLeft(movePoint);
            break;
        default:
            break;
        }
        widthInfoRect.setLocation(rect0.x(), rect0.y());
        widthInfoRect.setText(tr("%1 * %2").arg(rect0.width()).arg(rect0.height()));   //将截图选区的长宽信息显示在widthinforect中
    }
    if(draggable)                                                               //拖动调整可用时，进行调整过程绘图，并修改框选区域
    {
        if(!isDragging)                                                         //如果不在拖动状态时，仅更新光标样式；拖动时按类型更新拖动过程
        {
            RECTAREA CursorType = pointInWhere(event->pos());                   //根据光标所在区域，设置光标样式
            if(CursorType == RECT0)                                             //+ 全方位拖动
            {
                setCursor(Qt::SizeAllCursor);
            }
            else if((CursorType == RECT1) ||(CursorType == RECT3))              //➘下斜方向拖动
            {
                setCursor(Qt::SizeBDiagCursor);
            }
            else if((CursorType == RECT2) ||(CursorType == RECT4))              //➚上斜方向拖动
            {
                setCursor(Qt::SizeFDiagCursor);
            }
            else if((CursorType == TOP) ||(CursorType == BOTTOM))               //↕ 上下拖动样式
            {
                setCursor(Qt::SizeVerCursor);
            }
            else if((CursorType == LEFT) ||(CursorType == RIGHT))               //↔ 左右拖动样式
            {
                setCursor(Qt::SizeHorCursor);
            }
            else
            {
                setCursor(Qt::ArrowCursor);                                     //常规样式
            }
            type = CursorType;                                                  //记录类型
        }
        else
        {
            if(editorBar)
            {
                editorBar->hide();
            }
            cutPoint = rect0.topLeft();                                     //记录框选区域的当前坐标
            movePoint = event->pos();                                       //当前鼠标位置作为拖动终点
            qreal width = rect0.width();
            qreal height = rect0.height();
            qreal X = movePoint.x() - pressedPoint.x();                     //计算移动的差值
            qreal Y = movePoint.y() - pressedPoint.y();
            QPointF temp;
            switch (type) {                                                 //根据类型处理
            case RECT0:                                                     //拖动整个框选区域
                temp = movePoint - pressedPoint;
                break;
            case RECT1:                                                     //拖动右上角
                temp.setX(0);
                temp.setY(Y);
                rect0.setHeight(height - Y);
                rect0.setWidth(width + X);
                break;
            case RECT2:                                                     //拖动右下角
                temp.setX(0);
                temp.setY(0);
                rect0.setHeight(height + Y);
                rect0.setWidth(width + X);
                break;
            case RECT3:                                                     //拖动左下角
                temp.setX(X);
                temp.setY(0);
                rect0.setHeight(height + Y);
                rect0.setWidth(width - X);
                break;
            case RECT4:                                                     //拖动左上角
                temp.setX(X);
                temp.setY(Y);
                rect0.setHeight(height - Y);
                rect0.setWidth(width - X);
                break;
            case TOP:                                                       //拖动上边线
                temp.setX(0);
                temp.setY(Y);
                rect0.setHeight(height - Y);
                rect0.setWidth(width - 0);
                break;
            case LEFT:                                                      //拖动左边线
                temp.setX(X);
                temp.setY(0);
                rect0.setHeight(height - 0);
                rect0.setWidth(width - X);
                break;
            case BOTTOM:                                                    //拖动下边线
                temp.setX(0);
                temp.setY(0);
                rect0.setHeight(height + Y);
                rect0.setWidth(width - 0);
                break;
            case RIGHT:                                                     //拖动右边线
                temp.setX(0);
                temp.setY(0);
                rect0.setHeight(height - 0);
                rect0.setWidth(width + X);
                break;
            default:
                break;
            }
            rect0.moveTo(cutPoint + temp);
            widthInfoRect.setLocation(rect0.x(), rect0.y());                //重新设置尺寸显示条
            widthInfoRect.setText(tr("%1 * %2").arg(rect0.width()).arg(rect0.height()));
            pressedPoint = event->pos();
        }
    }
    repaint();
}

//键盘按下事件，按ESC退出程序
void Screen::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_N)
    {
        hide();
        QThread::msleep(500);
        QScreen *scrPix = QGuiApplication::primaryScreen();             //创建一个屏幕对象
        pixmap = scrPix->grabWindow(0);                                 //捕获一张指定窗口0（全屏）的像素图
        showFullScreen();                                               //设置全屏显示
        graphRatio = pixmap.devicePixelRatio();                         //获取设备缩放比
        globalPath.lineTo(pixmap.width(), 0);                           //绘图路径闭环，即整个屏幕
        globalPath.lineTo(pixmap.width(), pixmap.height());
        globalPath.lineTo(0, pixmap.height());
        globalPath.lineTo(0, 0);
        regionSelectable = false;
        draggable = false;
        isDragging = false;
    }
    if(event->key() == Qt::Key_Escape)
    {
        quit();
    }
}

//处理截图工具控制条
void Screen::handleEditorSignal(controlType type)
{
    if(!editorLabel->haveSet())     //如果没有设置ImageLabel，则进行设置，防止重新传入图片
    {
        setImageLabel();            //设置ImageLabel以后，可以对截图进行简单编辑处理
    }
    switch (type) {
    case SAVE:                      //将图片保存
        qDebug() << "SAVE";
        savePixmap();
        break;
    case CANCEL:
        qDebug() << "CANCEL";
        quit();
        break;
    case FINISH:
        qDebug() << "FINISH";
        copyToClipboard();
        break;
    case DRAWRECT:
        qDebug() << "DRAWRECT";
        break;
    case DRAWROUND:
        qDebug() << "DRAWROUND";
        break;
    case DRAWARROW:
        qDebug() << "DRAWARROW";
        break;
    case PEN:
        qDebug() << "PEN";
        break;
    case MOSAIC:
        qDebug() << "MOSAIC";
        break;
    case TEXT:
        qDebug() << "TEXT";
        break;
    case RETURN:
        qDebug() << "RETURN";
        break;
    default:
        break;
    }

}







