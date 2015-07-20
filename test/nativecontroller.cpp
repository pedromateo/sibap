#include "nativecontroller.h"

#include <QMessageBox>
#include <iostream>

#include "ui_mainwindow.h"

NativeController::NativeController()
{
}


///
///
/// CALIBRATION


void NativeController::on_pb_calibration_apply_clicked(Ui::MainWindow *ui)
{

}

void NativeController::on_pb_calibration_reset_clicked(Ui::MainWindow *ui)
{
    ui->di_nrLevel->setValue(0);
    ui->di_volumeLevel->setValue(0);
    ui->di_ampliLevel->setValue(0);
    ui->di_signalEcho->setValue(0);
    ui->hs_balanceLevel->setValue(50);
    ui->sb_bufferSize->setValue(0);
    ui->sb_sampleRate->setValue(0);
    ui->pb_left->setValue(0);
    ui->pb_right->setValue(0);
}

void NativeController::on_pb_right_valueChanged(Ui::MainWindow *ui,int v)
{
    /*int value = (ui->lcdNumber->value() +
                 ui->lcdNumber_2->value() +
                 ui->lcdNumber_3->value() +
                 ui->lcdNumber_4->value()) / 4 +
                ui->sb_bufferSize->value() / 2 +
                ui->sb_sampleRate->value() / 2;
    value = value * ui->hs_balanceLevel->value() / 100;
    ui->pb_right->setValue(value);*/
}

void NativeController::on_pb_left_valueChanged(Ui::MainWindow *ui,int v)
{
    /*int value = (ui->lcdNumber->value() +
                 ui->lcdNumber_2->value() +
                 ui->lcdNumber_3->value() +
                 ui->lcdNumber_4->value()) / 4 +
                ui->sb_bufferSize->value() / 2 +
                ui->sb_sampleRate->value() / 2;

    value = value * (100 - ui->hs_balanceLevel->value()) / 100;
    ui->pb_left->setValue(value);*/
}

///
///
/// SONAR

void NativeController::on_pb_sonar_apply_clicked(Ui::MainWindow *ui)
{
    int selected = ui->listWidget_2->currentRow();
    if (selected == 0)
    {
        ui->dial_5->setValue(30);
        ui->dial_6->setValue(40);
        ui->dial_7->setValue(50);
        ui->checkBox->setChecked(true);
        ui->checkBox_2->setChecked(true);
        ui->checkBox_3->setChecked(false);
        ui->checkBox_4->setChecked(true);
    }
    else if (selected == 1)
    {
        ui->dial_5->setValue(25);
        ui->dial_6->setValue(89);
        ui->dial_7->setValue(60);
        ui->checkBox->setChecked(false);
        ui->checkBox_2->setChecked(false);
        ui->checkBox_3->setChecked(false);
        ui->checkBox_4->setChecked(true);}
    else if (selected == 2)
    {
        ui->dial_5->setValue(50);
        ui->dial_6->setValue(50);
        ui->dial_7->setValue(50);
        ui->checkBox->setChecked(true);
        ui->checkBox_2->setChecked(true);
        ui->checkBox_3->setChecked(true);
        ui->checkBox_4->setChecked(true);}
    else if (selected == 3)
    {
        ui->dial_5->setValue(90);
        ui->dial_6->setValue(85);
        ui->dial_7->setValue(92);
        ui->checkBox->setChecked(false);
        ui->checkBox_2->setChecked(true);
        ui->checkBox_3->setChecked(false);
        ui->checkBox_4->setChecked(false);
    }
}

void NativeController::on_pb_sonar_save_clicked(Ui::MainWindow *ui)
{
    QMessageBox msgBox;
    msgBox.setText("The configuration has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
}

///
///
/// SONOBUOY

void NativeController::on_pb_sonobuoy_add_clicked(Ui::MainWindow *ui)
{
    QString separator = "_";
    QString id;
    id = "BUOY_type" +
         ui->comboBoxBuoyType->currentText() + separator +
         ui->spinBox->text() + separator +
         ui->spinBox_2->text() + separator +
         ui->spinBox_3->text() + separator +
         ui->spinBox_4->text() + separator +
         ui->spinBox_5->text() + separator +
         ui->spinBox_6->text() + separator +
         ui->spinBox_7->text() + separator +
         ui->spinBox_8->text() + ";";

    ui->listWidget->addItem(id);

}

void NativeController::on_pb_sonobuoy_reset_clicked(Ui::MainWindow *ui)
{
    ui->comboBoxBuoyType->setCurrentIndex(0);
    ui->spinBox->setValue(0);
    ui->spinBox_2->setValue(0);
    ui->spinBox_3->setValue(0);
    ui->spinBox_4->setValue(0);
    ui->spinBox_5->setValue(0);
    ui->spinBox_6->setValue(0);
    ui->spinBox_7->setValue(0);
    ui->spinBox_8->setValue(0);
}

void NativeController::on_pb_sonobuoy_delete_clicked(Ui::MainWindow *ui)
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

int balance = 0;
void NativeController::on_hs_balanceLevel_sliderMoved(Ui::MainWindow *ui,int balance)
{
    //control.setBalanceLevel(balance);
    //power = control.calculatePowerOutput();
    //setChannelsValue(power.left, power.right);
    //ui->pb_left->setValue(power.left);
}
