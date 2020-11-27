#ifndef SAFEQUEUE_HPP_
#define SAFEQUEUE_HPP_

#include <queue>
#include <list>
#include <mutex>
#include <thread>
#include <cstdint>
#include <condition_variable>

template <class T, class Container = std::list<T>>
class SafeQueue
{
    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef Container container_type;

public:
    SafeQueue() = default;
    SafeQueue(SafeQueue&& sq)
    {
        m_queue = std::move(sq.m_queue);
    }
    SafeQueue(const SafeQueue& sq)
    {
        std::lock_guard<std::mutex> lock(sq.m_mutex);
        m_queue = sq.m_queue;
    }

    ~SafeQueue()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
    }

    void set_max_num_items(unsigned int max_num_items)
    {
        m_max_num_items = max_num_items;
    }

    bool push(const value_type& item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_max_num_items > 0 && m_queue.size() > m_max_num_items)
            return false;

        m_queue.push(item);
        m_condition.notify_one();
        return true;
    }

    bool push(const value_type&& item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_max_num_items > 0 && m_queue.size() > m_max_num_items)
            return false;

        m_queue.push(item);
        m_condition.notify_one();
        return true;
    }

    void pop(value_type& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]()
            {
                return !m_queue.empty();
            });
        item = m_queue.front();
        m_queue.pop();
    }

    void move_pop(value_type& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() 
            {
                return !m_queue.empty();
            });
        item = std::move(m_queue.front());
        m_queue.pop();
    }

    bool try_pop(value_type& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return false;

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    bool try_move_pop(value_type& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return false;

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool timeout_pop(value_type& item, std::uint64_t timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
        {
            if (timeout == 0)
                return false;

            if (m_condition.wait_for(lock, std::chrono::microseconds(timeout)) == std::cv_status::timeout)
                return false;
        }

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    bool timeout_move_pop(value_type& item, std::uint64_t timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
        {
            if (timeout == 0)
                return false;

            if (m_condition.wait_for(lock, std::chrono::microseconds(timeout)) == std::cv_status::timeout)
                return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    size_type size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void swap(SafeQueue& sq)
    {
        if (this != &sq)
        {
            std::lock_guard<std::mutex> lock1(m_mutex);
            std::lock_guard<std::mutex> lock2(sq.m_mutex);
            m_queue.swap(sq.m_queue);

            if (!m_queue.empty())
                m_condition.notify_all();

            if (!sq.m_queue.empty())
                sq.m_condition.notify_all();
        }
    }

    SafeQueue& operator= (const SafeQueue& sq)
    {
        if (this != &sq)
        {
            std::lock_guard<std::mutex> lock1(m_mutex);
            std::lock_guard<std::mutex> lock2(sq.m_mutex);
            std::queue<T, Container> temp{ sq.m_queue };
            m_queue.swap(temp);

            if (!m_queue.empty())
                m_condition.notify_all();
        }

        return *this;
    }

    SafeQueue& operator= (SafeQueue&& sq)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue = std::move(sq.m_queue);

        if (!m_queue.empty())  m_condition.notify_all();

        return *this;
    }


private:
    std::queue<T, Container> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    unsigned int m_max_num_items = 0;
};

template <class T, class Container>
void swap(SafeQueue<T, Container>& q1, SafeQueue<T, Container>& q2)
{
    q1.swap(q2);
}
#endif