#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}*/

///
/// This example uses a controller implemented in NativeController class.
/// This is just to show the usage of other external classes.
/// Qt uses the model/view approach. This example does not follow this approach.
///

///
///
/// CALIBRATION

void MainWindow::on_pb_calibration_apply_clicked()
{
    std::cout << "calling controller method from another class..." << std::endl;
    _control.on_pb_calibration_apply_clicked(this->ui);
}

void MainWindow::on_pb_calibration_reset_clicked()
{
    _control.on_pb_calibration_reset_clicked(this->ui);
}

void MainWindow::on_pb_right_valueChanged(int v)
{
    _control.on_pb_right_valueChanged(this->ui,v);
}

void MainWindow::on_pb_left_valueChanged(int v)
{
    _control.on_pb_left_valueChanged(this->ui,v);
}

///
///
/// SONAR

void MainWindow::on_pb_sonar_apply_clicked()
{
    _control.on_pb_sonar_apply_clicked(this->ui);
}

void MainWindow::on_pb_sonar_save_clicked()
{
    _control.on_pb_sonar_save_clicked(this->ui);
}

///
///
/// SONOBUOY

void MainWindow::on_pb_sonobuoy_add_clicked()
{
    _control.on_pb_sonobuoy_add_clicked(this->ui);

}

void MainWindow::on_pb_sonobuoy_reset_clicked()
{
     _control.on_pb_sonobuoy_reset_clicked(this->ui);
}

void MainWindow::on_pb_sonobuoy_delete_clicked()
{
    _control.on_pb_sonobuoy_delete_clicked(this->ui);
}


void MainWindow::on_hs_balanceLevel_sliderMoved(int balance)
{
    _control.on_hs_balanceLevel_sliderMoved(this->ui,balance);
}
