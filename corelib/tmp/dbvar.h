#ifndef ______________DB_WV____H
#define ______________DB_WV____H

/**
* Темплейт для автоматизации изменений данных на запись.
*/

template <class T>
struct db_var
{
    T container;
    bool modified;
    db_var():modified(false) {}
    db_var(const T &b):container(b),modified(false) {}

    /// инициализация, например, из базы данных
    void init(const T& c)
    {
        container=c;
    }

    /// обновление из другого контейнера
    void update(const db_var<T>& c)
    {
        if(c.modified)
        {
            container=c.container;
            modified=true;
        }

    }
    /// получение ссылки на константный контейнер
    const T& const_get() const
    {
        return container;
    }
    /// получение ссылки на неконстантный контейнер с пометкой об изменении контейнера
    T&  get()
    {
        modified=true;
        return container;
    }
};
#endif
