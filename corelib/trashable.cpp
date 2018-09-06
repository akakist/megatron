#include "trashable.h"
void Trash::add(Trashable* p)
{
    container.push_back(p);
}

Trash::~Trash()
{
//    for(auto& z:container)
//        delete z;
}
