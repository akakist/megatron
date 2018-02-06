#ifndef GUIDSHANDLER_H
#define GUIDSHANDLER_H
#include "UCHandler.h"
#include <QObject>
#include "UCHandler.h"
class GuiUCHandler: public QObject, public UCHandler
{
    Q_OBJECT
public:
    GuiUCHandler(IInstance *ins);
    bool on_JsonRSP(const jsonRefEvent::JsonRSP*) {
        return false;
    }

#if  !defined(__OFFLINE__)
    void signal_connected();
    void signal_connect_failed();
    void signal_disconnected();

    void signal_authorized();

signals:
    void connected();
    void connect_failed();
    void disconnected();
#endif

};

#endif // GuiUCHandler_H
