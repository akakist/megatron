#include "GuiUCHandler.h"
#include "tools_mt.h"
#include <QMessageBox>




GuiUCHandler::GuiUCHandler(IInstance *ins)
    : UCHandler(ins)
{

}
#if  !defined(__OFFLINE__)
void GuiUCHandler::signal_connect_failed()
{
    emit connect_failed();
}

void GuiUCHandler::signal_connected()
{
#if  !defined(__OFFLINE__)
    //do_subscribeForNewsAndPushs();
#endif

    emit connected();
    logErr2("signal_connected();");
}
void GuiUCHandler::signal_authorized()
{

}

void GuiUCHandler::signal_disconnected()
{
    emit disconnected();
    logErr2("signal_disconnected();");
}




#endif
