#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(QWidget *parent);

public:
    QImage sendImage();
    void loadedImage(bool send);
    bool haveSet();
    void drawArrow();                   //画箭头
    void drawRound();                   //画圆形
    void drawRect();                    //画方形
    void editText();                    //编辑文字
    void getMosaic();                   //打码
    void pencil();                      //铅笔
private:
    bool   set;
};

#endif // IMAGELABEL_H
