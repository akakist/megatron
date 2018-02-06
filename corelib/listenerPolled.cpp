#include "listenerPolled.h"
#include "mutexInspector.h"



ListenerPolled::~ListenerPolled()
{

}
ListenerPolled::ListenerPolled(UnknownBase *i, const std::string& name,IConfigObj* , const SERVICE_id& sid)
    :ListenerBase(i,name,sid)
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
            bool processed=false;

            for(std::vector<std::pair<eventhandler,void*> >::iterator i=handlers.begin(); i!=handlers.end(); ++i)
            {
                if(i->first(d[n].operator ->(),i->second))processed=true;
            }

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
                logErr2("ListenerPolled: unhandled event %s in %s",d[n]->dump().c_str(),listenerName.c_str());
                XPASS;

            }
        }
    }
    catch(std::exception &e)
    {
        logErr2("exception: %s %s ",e.what(),_DMI().c_str());
    }

    return;
}
void ListenerPolled::listenToEvent(const std::deque<REF_getter<Event::Base> >&e)
{
    M_LOCK(this);
    for(size_t i=0; i<e.size(); i++)
    {
        m_container_.push_back(e[i]);
    }

}

void ListenerPolled::listenToEvent(const REF_getter<Event::Base>& e)
{

    XTRY;

    M_LOCK(this);
    m_container_.push_back(e);
    XPASS;


}
