#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <nativecontroller.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    //void changeEvent(QEvent *e);

protected:
    Ui::MainWindow *ui;
    NativeController _control;

private slots:

    ///
    /// CALIBRATION

    void on_pb_calibration_apply_clicked();
    void on_pb_calibration_reset_clicked();
    void on_pb_right_valueChanged(int v);
    void on_pb_left_valueChanged(int v);

    ///
    /// SONAR

    void on_pb_sonar_apply_clicked();
    void on_pb_sonar_save_clicked();


    ///
    /// SONOBUOY

    void on_pb_sonobuoy_add_clicked();
    void on_pb_sonobuoy_reset_clicked();
    void on_pb_sonobuoy_delete_clicked();
    void on_hs_balanceLevel_sliderMoved(int balance);
};

#endif // MAINWINDOW_H
