#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTimer>
#include "tools_mt.h"
#include <QFileDialog>
#include "st_rsa.h"
#include "js_utils.h"
#include "st_FILE.h"
#include "configDB.h"
#include "gTry.h"
#ifndef __ANDROID__
//#include <QWebEngineView>
#endif
#include "megatron_config.h"


#include <QDesktopServices>

#include "GuiUCHandler.h"
#include "Events/DFS/Referrer/Pong.h"
#include "Events/DFS/Referrer/InitClient.h"
#include "Events/DFS/Referrer/UpdateConfigREQ.h"
#include "Events/DFS/Referrer/UpdateConfigRSP.h"
#include "megatron.h"
extern megatron* mega;
/*
void registerJsonNoRefClientService(const char* pn);
void registerJsonRefClientService(const char* pn);
void registerDFSReferrerService(const char* pn);
void registerJsonLocalRefClientService(const char* pn);
void registerDFSReferrerService(const char* pn);
*/
//GuiUCHandler *handler_static;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),handler(NULL)
{

    GTRY;
    ui->setupUi(this);

    srand(iUtils->getNow().get()%0xffffffff);
    timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(100);
//    ConfigDB_private c;

    //makeMenu(false);
    iUtils->registerEvent(dfsReferrerEvent::UpdateConfigRSP::construct);
    if(!handler)
    {
        handler=new GuiUCHandler(mega->iInstance);
        connectToHandler();
        std::set<msockaddr_in> caps;
        caps.insert(msockaddr_in(CAPS_ADDRESS));
        handler->sendEvent(ServiceEnum::ReferrerClient,new dfsReferrerEvent::InitClient(caps,handler));


    }

    GCATCH;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimer()
{

    MUTEX_INSPECTOR;
    XTRY;
    IObjectProxyPolled* op=static_cast<IObjectProxyPolled*>
                           (mega->iInstance->getServiceOrCreate(ServiceEnum::ObjectProxyPolled)->cast(UnknownCast::IObjectProxyPolled));
    op->poll();
    XPASS;
}





void MainWindow::connected()
{

    {
        ConfigDB_private c;
        std::string email=c.get_string("login_session","");
        if(email!="")
        {
            //logErr2("recvd email %s",email.c_str());

            ui->stackedWidget->setCurrentWidget(ui->pageMenu);

            {
            }
            return;
        }

    }    {
    }

}
void MainWindow::disconnected()
{
    //ui->widgetAuth->setEnabled(false);
    //makeMenu(false);
}
void MainWindow::connect_failed()
{
    //logErr2("void MainWindow::connect_failed()");
}


void MainWindow::connectToHandler()
{
    GTRY;
    if(!checkHandler())return;
    connect(handler,SIGNAL(connected()),this,SLOT(connected()));
    connect(handler,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(handler,SIGNAL(connect_failed()),this,SLOT(connect_failed()));

    GCATCH;
}
bool MainWindow::checkHandler()
{
    if(!handler)
    {
        QMessageBox::warning(this,"Ошибка","Не выполнен вход");
        return false;
    }
    return true;
}






















void MainWindow::on_pushButton_updateConfig_clicked()
{
    REF_getter<Event::Base> et=new dfsReferrerEvent::ToplinkDeliverREQ(
        ServiceEnum::DFSReferrer,
        new dfsReferrerEvent::UpdateConfigREQ(ui->plainTextEdit_updateConfig->toPlainText().toStdString(),handler),handler);
    handler->sendEvent(ServiceEnum::ReferrerClient,et);

}

