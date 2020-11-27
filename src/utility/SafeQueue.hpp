#ifndef SAFEQUEUE_HPP_
#define SAFEQUEUE_HPP_

template <class T>
class SafeQueue
{
public:
    bool produce(const T& data)
    {
        {
            std::lock_guard <std::mutex> lock(queue_mutex);
            internal_queue.push(data);
        }
        return true;
    }

    bool try_consume(T& item)
    {
        {
            std::lock_guard <std::mutex> lock(queue_mutex);
            if (internal_queue.empty())
                return false;

            item = internal_queue.front();
            internal_queue.pop();
        }
        return true;
    }

    void clear() 
    {
        {
            std::lock_guard <std::mutex> lock(queue_mutex);
            while (!internal_queue.empty()) {
                internal_queue.pop();
            }
        }
    }

    size_t size()
    {
        size_t ret = 0;
        {
            std::lock_guard <std::mutex> lock(queue_mutex);
            ret = internal_queue.size();
        }
        return ret;
    }

    bool empty()
    {
        bool ret = true;
        {
            std::lock_guard <std::mutex> lock(queue_mutex);
            ret = internal_queue.empty();
        }
        return ret;
    }

private:
    std::queue<T> internal_queue;
    std::mutex queue_mutex;
};
#endif