#include "listenerPolled.h"
#include "IUtils.h"
#include "mutexInspector.h"
#include "colorOutput.h"


ListenerPolled::~ListenerPolled()
{

    m_container_.clear();
}
ListenerPolled::ListenerPolled(const std::string& name, const SERVICE_id& sid)
    :ListenerBase(name,sid)
{
}

void ListenerPolled::poll()
{

    try
    {
        std::deque<REF_getter<Event::Base> > d;
        {
            M_LOCK(this);
            d=m_container_;
            m_container_.clear();
        }
        for(size_t n=0; n<d.size(); n++)
        {
            try {
                bool processed=false;

//                for(auto& i:handlers)
//                {
//                    if(i.first(d[n].get(),i.second))processed=true;
//                }

                if(!processed)
                {
                    XTRY;
                    if(handleEvent(d[n]))
                    {
                        processed=true;
                    }
                    XPASS;
                }
                if(!processed)
                {
                    XTRY;
                    logErr2("ListenerPolled: unhandled event %s in %s",d[n]->dump().toStyledString().c_str(),listenerName_.c_str());
                    XPASS;

                }
            }
            catch(const std::exception& e)
            {
                logErr2("ListenerPolled exception: " RED2("%s") " %s %s",e.what(),_DMI().c_str(),d[n]->dump().toStyledString().c_str());

            }
        }
    }
    catch(const std::exception &e)
    {
        logErr2("ListenerPolled exception: %s %s ",e.what(),_DMI().c_str());
    }

    return;
}

void ListenerPolled::listenToEvent(const REF_getter<Event::Base>& e)
{

    XTRY;

    M_LOCK(this);
    m_container_.push_back(e);
    XPASS;


}
void ListenerPolled::deinit()
{

}
