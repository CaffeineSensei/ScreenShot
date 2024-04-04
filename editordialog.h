#ifndef EDITORDIALOG_H
#define EDITORDIALOG_H

#include <QDialog>

enum controlType
{
    SAVE,
    CANCEL,
    FINISH,
    DRAWRECT,
    DRAWROUND,
    DRAWARROW,
    PEN,
    MOSAIC,
    TEXT,
    RETURN
};

namespace Ui {
class EditorDialog;
}

class EditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorDialog(QWidget *parent = nullptr);
    ~EditorDialog();
signals:
    void editorButtonPressed(controlType);
private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonText_clicked();
    void on_pushButtonMosaic_clicked();
    void on_pushButtonPen_clicked();
    void on_pushButtonArrow_clicked();
    void on_pushButtonCircle_clicked();
    void on_pushButtonSquare_clicked();
    void on_pushButtonReturn_clicked();

private:
    Ui::EditorDialog *ui;
    controlType type;
};

#endif // EDITORDIALOG_H
