#pragma once
/*
#define COUNT 10
static OBJ queue[COUNT];
static int enque_idx = 1;
static int deque_idx = 0;

#define IS_FULL() (enque_idx == deque_idx)
#define IS_EMPTY() ((enque_idx + COUNT - deque_idx) % COUNT == 1)

void insert(OBJ o)
{
    if(!IS_FULL())
    {
        queue[enque_idx] = o;
        enque_idx = (enque_idx + 1) % COUNT;
    }
}

void deque(OBJ *o)
{
    if(!IS_EMPTY())
    {
        deque_idx = (deque_idx + 1) % COUNT;
        *o = queue[deque_idx];
    }
}
*/

template<class T>
class CIRCULAR_Q
{
    T *m_items;
    int m_max_items;
    int m_enque_idx = 1;
    int m_deque_idx = 0;

public:
    CIRCULAR_Q(int q_size)
        : m_max_items(q_size)
        , m_enque_idx(1)
        , m_deque_idx(0)
        , m_items(nullptr)
    {
        m_items = new T[m_max_items];
    }
    ~CIRCULAR_Q()
    {
        delete [] m_items;
    }
    bool isFull() const
    {
        return (m_enque_idx == m_deque_idx);
    }
    bool isEmpty() const
    {
        return ((m_enque_idx + m_max_items - m_deque_idx) % m_max_items == 1);
    }
    int count() const
    {
        return (m_enque_idx > m_deque_idx) ?
                    m_enque_idx - m_deque_idx :
                    m_max_items - (m_deque_idx - m_enque_idx);
    }
    void add(T &o)
    {
        if(!isFull())
        {
            m_items[m_enque_idx] = o;
            m_enque_idx = (m_enque_idx + 1) % m_max_items;
        }
    }
    T& remove()
    {
        if(!isEmpty())
        {
            m_deque_idx = (m_deque_idx + 1) % m_max_items;
            return m_items[m_deque_idx];
        }
        return m_items[0];
    }
};


