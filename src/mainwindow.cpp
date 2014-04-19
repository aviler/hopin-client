#include <QAbstractSlider>
#include <QDebug>
#include <QLabel>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef WIN32
    // A lame way to make interface on Windows render better
    // by reducing the font size on EVERY QLabel
    QFont f( "Arial", 12, QFont::Bold);
    QFont f2( "Arial", 12, QFont::Normal);
    QFont f3( "Arial", 30, QFont::Bold);
    // On login_wdg
    ui->email_lbl->setFont(f);
    ui->pass_lbl->setFont(f);
    ui->login_btn->setFont(f);
    ui->email_txt->setFont(f2);
    ui->pass_txt->setFont(f2);
    // On main_wdg
    ui->user_lbl->setFont(f);
    ui->useremail_lbl->setFont(f);
    ui->hopins_lbl->setFont(f);
    ui->idletime_lbl->setFont(f);
    ui->percent_lbl->setFont(f);
    ui->start_btn->setFont(f);
    ui->stop_btn->setFont(f);
    ui->warn_lbl->setFont(f2);
    ui->hopinsvalue_lbl->setFont(f3);

#else
    // UNIX code here
    // nothing to do here
#endif

    // Little hack so on mac we can find were ths plugins are
#if defined(Q_OS_MAC)
    QDir pluginsDir(qApp->applicationDirPath());
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
    }
 #endif

    // this is stupid, we should be able to connect this directly at Controller
    connect(ui->login_btn, &QPushButton::clicked,
            this, &MainWindow::loginBtnClicked);

    connect(ui->idletime_sldr, &QSlider::valueChanged,
            this, &MainWindow::idletimeSldrValueChanged);

    connect(ui->start_btn, &QPushButton::clicked,
            this, &MainWindow::startBtnClicked);

    connect(ui->stop_btn, &QPushButton::clicked,
            this, &MainWindow::stopBtnClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hideLoginWdg()
{
    ui->login_wdg->hide();
    ui->pass_txt->clear();
}

void MainWindow::showLoginWdg()
{
    ui->login_wdg->show();
    // Make sure that some components were cleared
    ui->login_btn->setEnabled(true);
    ui->loginwarn_lbl->setText("");
    ui->pass_txt->clear();
}

void MainWindow::hideMainWdg()
{
    ui->main_wdg->hide();
}

void MainWindow::showMainWdg()
{
    ui->main_wdg->show();
}

void MainWindow::setIdletimeSldrMax(int value)
{
    ui->idletime_sldr->setMaximum(value);
    ui->idletime_sldr->setValue(value);
}

void MainWindow::setHopinsAmount(QString hopins)
{
    ui->hopinsvalue_lbl->setText(hopins);
}

void MainWindow::setLoginWarning(QString warn)
{
    ui->loginwarn_lbl->setText(warn);
}

void MainWindow::loginBtnSetEnabled(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
}

void MainWindow::setUserEmail(QString useremail)
{
    ui->useremail_lbl->setText(useremail);
}

void MainWindow::setStatusLabel(QString status)
{
    ui->status_lbl->setText(status);
}

void MainWindow::loginBtnClicked()
{
    ui->login_btn->setEnabled(false);
    emit login(ui->email_txt->text(), ui->pass_txt->text());
}

void MainWindow::idletimeSldrValueChanged(int pos)
{
    if(pos == 0) {
        ui->start_btn->setEnabled(false);
        ui->percent_lbl->setText("0 %");
    } else {
        ui->start_btn->setEnabled(true);
        //calculate the percentage to show
        float max = ui->idletime_sldr->maximum();
        float percent = (pos * 100) / max;
        ui->percent_lbl->setText(QString::number(percent) + " %");
    }
}

void MainWindow::startBtnClicked()
{
    emit start(ui->idletime_sldr->sliderPosition());
}

void MainWindow::stopBtnClicked()
{
    emit stop();
}

void MainWindow::setStoppedState()
{
    ui->stop_btn->setEnabled(false);
    ui->idletime_sldr->setEnabled(true);
    ui->percent_lbl->setEnabled(true);
    ui->start_btn->setEnabled(true);
}

void MainWindow::setMiningState()
{
    ui->start_btn->setEnabled(false);
    ui->idletime_sldr->setEnabled(false);
    ui->percent_lbl->setEnabled(false);
    ui->stop_btn->setEnabled(true);
}
