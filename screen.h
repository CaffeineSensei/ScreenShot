#ifndef SCREEN_H
#define SCREEN_H
#include "cutrect.h"
#include "editordialog.h"
#include "imagelabel.h"
#include <QWidget>
#include <QPainterPath>

//区域枚举
enum RECTAREA
{                           //              top
    RECT1,                  // rect4┌────────────────────┐rect1
    RECT2,                  //      │                    │
    RECT3,                  //  left│      rect0         │right
    RECT4,                  //      │                    │
    RECT0,                  // rect3└────────────────────┘rect2
    OUTOFRECT,              //              bottom
    TOP,                    // out of rect 不在任何区域
    LEFT,
    BOTTOM,
    RIGHT,
};

enum LINE
{
    TOPLINE,
    LEFTLINE,
    BOTTOMLINE,
    RIGHTLINE,
    NO
};



class Screen : public QWidget
{
    Q_OBJECT

public:
    Screen(QWidget *parent = nullptr);
    ~Screen();

public:
    QPainterPath getCutPath();                          //获取框选的截图区的闭环路径
    void drawCutAreaFourAngles(QPainter &painter);      //绘制框选区四角的小方块
    int comparePoint(QPointF &p1, QPointF &p2);         //比较两点的大小,用于判断框选时的方向
    bool pointInRect(QPoint &p, QRectF &r);             //判断点是否位于区域内
    LINE pointNearLine(QPoint &p,QRectF &r);            //判断点从外侧靠近区域的哪一条边线
    RECTAREA pointInWhere(QPoint p);
    QPixmap cutGrabPixmap();                            //从整张像素图截取想要的，即框选出来的区域
    void setImageLabel();
    void copyToClipboard();
    void savePixmap();
    void quit();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleEditorSignal(controlType type);           //处理控制条按下的信号

private:
    QPixmap         pixmap;                         //捕捉到的全屏像素图
    qreal           graphRatio;                     //缩放比，系统不是100%时，会导致坐标与尺寸不正确
    QPainterPath    globalPath;                     //绘图路径
    QPointF         pressedPoint;                   //框选时按下鼠标的坐标
    QPointF         movePoint;                      //框选后鼠标按键松开的坐标
    CutRect         widthInfoRect;                  //用于显示框选区域尺寸的方框
    QRectF          rect1;                          //第一象限的小方块
    QRectF          rect2;                          //第二象限的小方块
    QRectF          rect3;                          //第三象限的小方块
    QRectF          rect4;                          //第四象限的小方块
    QRectF          rect0;                          //框选区域矩形
    QPointF         cutPoint;                       //框选后的坐标，用来定位即将生成的截图处理窗口
    EditorDialog*   editorBar;
    ImageLabel*     editorLabel;
    bool            regionSelectable;               //可执行框选截图区标志
    bool            draggable;                      //可拖动框选边线标志
    bool            isDragging;
    RECTAREA        type;
};
#endif // SCREEN_H
