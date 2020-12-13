#ifndef SAFEQUEUE_HPP_
#define SAFEQUEUE_HPP_

template <class T>
class SafeQueue
{
private:
	std::queue<T> internal_queue;
	std::mutex queue_mutex;

public:
	SafeQueue();
	~SafeQueue();
	bool produce(const T& data);
	bool try_consume(T& item);
	void clear();
	size_t size();
	bool empty();
};


template <class T>
SafeQueue<T>::SafeQueue()
{}

template <class T>
SafeQueue<T>::~SafeQueue()
{}

template <class T>
bool SafeQueue<T>::produce(const T& data)
{
	{
		std::lock_guard <std::mutex> lock(queue_mutex);
		internal_queue.push(data);
	}
	return true;
}

template <class T>
bool SafeQueue<T>::try_consume(T& item)
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

template <class T>
void SafeQueue<T>::clear()
{
	{
		std::lock_guard <std::mutex> lock(queue_mutex);
		while (!internal_queue.empty())
		{
			internal_queue.pop();
		}
	}
}

template <class T>
size_t SafeQueue<T>::size()
{
	size_t ret = 0;
	{
		std::lock_guard <std::mutex> lock(queue_mutex);
		ret = internal_queue.size();
	}
	return ret;
}

template <class T>
bool SafeQueue<T>::empty()
{
	bool ret = true;
	{
		std::lock_guard <std::mutex> lock(queue_mutex);
		ret = internal_queue.empty();
	}
	return ret;
}
#endif