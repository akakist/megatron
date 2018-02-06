#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "jsonHandler.h"
#include <QLabel>
#include "UCHandler.h"
#include "GuiUCHandler.h"
#include "GuiUCHandler.h"
namespace Ui {
    class MainWindow;
}
struct auth2
{

};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onTimer();

    //void generateMultipleCodes(QString dir, QString prefix, int ncount, int amount);


    void connected();
    void disconnected();
    void connect_failed();




























    void on_pushButton_updateConfig_clicked();


private:
    Ui::MainWindow *ui;
    QTimer *timer;


    void connectToHandler();
    bool checkHandler();

    GuiUCHandler *handler;


};

#endif // MAINWINDOW_H
