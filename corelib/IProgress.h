#pragma once

#include <string>
#include "route_t.h"

struct IProgress
{
    virtual void msg(const std::string& msg)=0;
    virtual void msg(const std::string& msg,const std::string& fake_msg)=0;
    virtual void nop(const std::string& msg)=0;
    virtual void nop(const std::string& msg,const std::string& fake_msg)=0;
    virtual void failed(const std::string& msg)=0;
    virtual void setRoute(const route_t& r)=0;
    virtual void setPosition(int i,int max_scale_steps)=0;
};




