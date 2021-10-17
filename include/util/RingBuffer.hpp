#pragma once
#include <mutex>

template <typename T>
class RingBuffer
{
private:
	std::condition_variable consume_condition;
	std::condition_variable produce_condition;
	int head;
	int tail;
	T* buffer;
	int buf_size;
	std::mutex buf_mutex;

public:
	RingBuffer(int size);
	int size();
	bool is_empty();
	bool is_full();
	T read();
	void write(T data);
};


template <typename T>
RingBuffer<T>::RingBuffer(int size)
{
	if (size < 2)
	{
		std::cerr << "invalid size" << std::endl;
	}
	this->buf_size = size + 1;
	buffer = new T[this->buf_size];
	head = 0;
	tail = 0;
}

template <typename T>
int RingBuffer<T>::size()
{
	return (tail + buf_size - head) % buf_size;
}

template <typename T>
bool RingBuffer<T>::is_empty()
{
	return head == tail;
}

template <typename T>
bool RingBuffer<T>::is_full()
{
	return head == ((tail + 1) % buf_size);
}

template <typename T>
T RingBuffer<T>::read()
{
	std::unique_lock<std::mutex> lock(this->buf_mutex);
	consume_condition.wait(lock, [=]
	{
		return !is_empty();
	});
	T data = buffer[head];
	head = (head + 1) % buf_size;
	lock.unlock();
	produce_condition.notify_one();
	return data;
}

template <typename T>
void RingBuffer<T>::write(T data)
{
	std::unique_lock<std::mutex> lock(this->buf_mutex);
	produce_condition.wait(lock, [=]
	{
		return !is_full();
	});
	buffer[tail] = data;
	tail = (tail + 1) % buf_size;
	lock.unlock();
	consume_condition.notify_one();
}