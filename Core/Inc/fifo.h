#pragma once

#include <cstddef>
#include <cstdint>

template<typename T, std::size_t N>
class StaticFifo {
public:
    StaticFifo() : head(0), tail(0), count(0) {}

    bool push(const T& value) {
        if (isFull()) {
            return false;
        }
        buffer[head] = value;
        head = (head + 1) % N;
        ++count;
        return true;
    }

    std::size_t pushMulti(const T* values, std::size_t len) {
        std::size_t pushed = 0;
        while (pushed < len && !isFull()) {
            buffer[head] = values[pushed];
            head = (head + 1) % N;
            ++count;
            ++pushed;
        }
        return pushed;
    }

    bool pop(T& out) {
        if (isEmpty()) {
            return false;
        }
        out = buffer[tail];
        tail = (tail + 1) % N;
        --count;
        return true;
    }

    std::size_t popMulti(T* out, std::size_t len) {
        std::size_t popped = 0;
        while (popped < len && !isEmpty()) {
            out[popped] = buffer[tail];
            tail = (tail + 1) % N;
            --count;
            ++popped;
        }
        return popped;
    }

    void clear() { head = 0; tail = 0; count = 0; }
    bool isEmpty() const { return count == 0; }
    bool isFull()  const { return count == N; }
    std::size_t size() const { return count; }
    std::size_t capacity() const { return N; }
    const T* getAddr()const { return &buffer[0]; }

private:
    T buffer[N] = { 0 };
    std::size_t head;
    std::size_t tail;
    std::size_t count;
};
