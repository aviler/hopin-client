#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void hideLoginWdg();
    void showLoginWdg();
    void hideMainWdg();
    void showMainWdg();
    void setIdletimeSldrMax(int value);
    void setHopinsAmount(QString hopins);
    void setLoginWarning(QString warn);
    void loginBtnSetEnabled(bool enabled);
    void setUserEmail(QString useremail);
    void setStatusLabel(QString status);
    void setStoppedState();
    void setMiningState();

private:
    Ui::MainWindow *ui;

signals:
    void login(const QString &email, const QString &password);
    void start(int numThreads);
    void stop();

public slots:
    void loginBtnClicked();
    void idletimeSldrValueChanged(int pos);
    void startBtnClicked();
    void stopBtnClicked();


};

#endif // MAINWINDOW_H
