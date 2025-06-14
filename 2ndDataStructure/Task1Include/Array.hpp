#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>

template <typename T>
class Array {
private:
    T* data;
    int capacity;
    int size;

    void resize() {
        capacity *= 2;
        T* newData = new T[capacity];
        for (int i = 0; i < size; i++)
            newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    // Constructor
    Array(int initialCapacity = 10)
        : capacity(initialCapacity), size(0) {
        data = new T[capacity];
    }

    // Deep Copy Constructor
    Array(const Array<T>& other)
        : capacity(other.capacity), size(other.size) {
        data = new T[capacity];
        for (int i = 0; i < size; i++)
            data[i] = other.data[i];
    }

    // Deep Copy Assignment Operator
    Array<T>& operator=(const Array<T>& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (int i = 0; i < size; i++)
                data[i] = other.data[i];
        }
        return *this;
    }

    ~Array() {
        delete[] data;
    }

    void add(const T& item) {
        if (size == capacity)
            resize();
        data[size++] = item;
    }

    T& get(int index) const {
        if (index < 0 || index >= size)
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    void set(int index, const T& item) {
        if (index < 0 || index >= size)
            throw std::out_of_range("Index out of range");
        data[index] = item;
    }

    int getSize() const {
        return size;
    }

    void removeLast() {
        if (size > 0)
            size--;
    }

    void clear() {
        size = 0;
    }

    T& operator[](int index) {
        return get(index);
    }

    const T& operator[](int index) const {
        return get(index);
    }
};

#endif
