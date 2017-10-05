#ifndef HITABLE_OBJECT_H
#define HITABLE_OBJECT_H

#include "hitable.h"
#include "object.h"
#include "quadtree.h"

#include <memory>
#include <utility>

class hitable_object : public object
{
protected:
    hitable_object(std::unique_ptr<hitable> p)
        : m_object(std::move(p))
    {
    }

public:
    hitable& get_object()
    {
        return *m_object;
    }

    const hitable& get_object() const
    {
        return *m_object;
    }
private:
    std::unique_ptr<hitable> m_object;
};

#endif // HITABLE_OBJECT_H
