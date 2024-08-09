#pragma once
#include <QMessageBox>
#define GTRY try{
#define GCATCH     }catch(const std::exception& e){QMessageBox::warning(this,"Error",e.what());}\
                    catch(const CommonError& e){QMessageBox::warning(this,"Error",e.what());}

