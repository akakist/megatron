#ifndef GTRY_H
#define GTRY_H
#include <QMessageBox>
#define GTRY try{
#define GCATCH     }catch(std::exception& e){QMessageBox::warning(this,"Error",e.what());}\
                    catch(CommonError& e){QMessageBox::warning(this,"Error",e.what());}

#endif // GTRY_H
