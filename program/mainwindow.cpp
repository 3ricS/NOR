#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QPushButton"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton* hi = new QPushButton();
    hi->setText("Ich bin hot!");
    hi->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
