#include "editordialog.h"
#include "ui_editordialog.h"

EditorDialog::EditorDialog(QWidget *parent)
    : QDialog(parent,Qt::FramelessWindowHint)
    , ui(new Ui::EditorDialog)
{
    ui->setupUi(this);
}

EditorDialog::~EditorDialog()
{
    delete ui;
}

void EditorDialog::on_pushButtonOK_clicked()
{
    type = FINISH;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonCancel_clicked()
{
    type = CANCEL;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonSave_clicked()
{
    type = SAVE;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonText_clicked()
{
    type = TEXT;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonMosaic_clicked()
{
    type = MOSAIC;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonPen_clicked()
{
    type = PEN;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonArrow_clicked()
{
    type = DRAWARROW;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonCircle_clicked()
{
    type = DRAWROUND;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonSquare_clicked()
{
    type = DRAWRECT;
    emit editorButtonPressed(type);
}


void EditorDialog::on_pushButtonReturn_clicked()
{
    type = RETURN;
    emit editorButtonPressed(type);
}

