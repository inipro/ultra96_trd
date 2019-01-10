#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QPixmap>
#include <QString>

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

//Open image function call
void MainWindow::openImage()
{
    //declare FileOpName as the choosen opened file name
    FileOpName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath(),tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    //Check if FileOpName exist or not
    if (!FileOpName.isEmpty()) {
        QImage image(FileOpName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Face Recognition"),
                                     tr("Cannot load %1.").arg(FileOpName));

            return;
        }

    }
    //function to load the image whenever fName is not empty
        if( FileOpName.size() )
        {
        imagerd = cvLoadImage(FileOpName.toLocal8Bit().data());
        QImage imageView = QImage((const unsigned char*)(imagerd->imageData), imagerd->width,imagerd->height,QImage::Format_RGB888).rgbSwapped();
        ui->label->setPixmap(QPixmap::fromImage(imageView));
        }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    openImage();
}
