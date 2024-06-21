#pragma once
#include <QMessageBox>
#define GTRY try{
#define GCATCH     }catch(std::exception& e){QMessageBox::warning(this,"Error",e.what());}\
                    catch(CommonError& e){QMessageBox::warning(this,"Error",e.what());}

