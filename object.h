#ifndef OBJECT_H
#define OBJECT_H

struct aabb;

class object
{
public:
    object()
        : m_id(s_next_id++)
    {
    }

    virtual ~object() = default;
    virtual aabb get_aabb() const = 0;

    int id() const
    {
        return m_id;
    }
private:
    int m_id;
    static int s_next_id;
};


#endif /* OBJECT_H */
