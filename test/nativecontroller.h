#ifndef NATIVECONTROLLER_H
#define NATIVECONTROLLER_H


namespace Ui {
    class MainWindow;
}


///
/// \brief The NativeController class
///
class NativeController {


public:
    NativeController();

    ///
    /// CALIBRATION

    void on_pb_calibration_apply_clicked(Ui::MainWindow *ui);
    void on_pb_calibration_reset_clicked(Ui::MainWindow *ui);
    void on_pb_right_valueChanged(Ui::MainWindow *ui, int v);
    void on_pb_left_valueChanged(Ui::MainWindow *ui, int v);

    ///
    /// SONAR

    void on_pb_sonar_apply_clicked(Ui::MainWindow *ui);
    void on_pb_sonar_save_clicked(Ui::MainWindow *ui);


    ///
    /// SONOBUOY

    void on_pb_sonobuoy_add_clicked(Ui::MainWindow *ui);
    void on_pb_sonobuoy_reset_clicked(Ui::MainWindow *ui);
    void on_pb_sonobuoy_delete_clicked(Ui::MainWindow *ui);
    void on_hs_balanceLevel_sliderMoved(Ui::MainWindow *ui, int balance);
};

#endif // NATIVECONTROLLER_H
