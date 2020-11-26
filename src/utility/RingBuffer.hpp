#ifndef _RING_BUFFER_
#define _RING_BUFFER_

namespace Guarneri {
    template <typename T>
    class RingBuffer {
    public:
        RingBuffer(int size) {
            if (size < 2) {
                std::cerr << "invalid size" << std::endl;
            }
            this->buf_size = size + 1;
            buffer = new T[this->buf_size];
            head = 0;
            tail = 0;
        }

        std::condition_variable consume_condition;
        std::condition_variable produce_condition;


    private:
        int head;
        int tail;
        T* buffer;
        int buf_size;
        std::mutex buf_mutex;

    public:
        int size() {
            return (tail + buf_size - head) % buf_size;
        }

        bool is_empty() {
            return head == tail;
        }

        bool is_full() {
            return head == ((tail + 1) % buf_size);
        }

        T read() {
            std::unique_lock<std::mutex> lock(this->buf_mutex);
            consume_condition.wait(lock, [=] { return !is_empty(); });
            T data = buffer[head];
            head = (head + 1) % buf_size;
            lock.unlock();
            produce_condition.notify_one();
            return data;
        }

        void write(T data) {
            std::unique_lock<std::mutex> lock(this->buf_mutex);
            produce_condition.wait(lock, [=] { return !is_full(); });
            buffer[tail] = data;
            tail = (tail + 1) % buf_size;
            lock.unlock();
            consume_condition.notify_one();
        }
    };
}
#endif