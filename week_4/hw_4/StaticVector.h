//
// Created by Amy Wang on 4/22/25.
//

#ifndef STATICVECTOR_H
#define STATICVECTOR_H

template<typename T, std::size_t N>
class StaticVector {
    T data[N];
    std::size_t total_size = 0;
public:
    bool push_back(const T& value) {
        if (total_size >=N) return false;
        data[total_size++] = value;
        return true;

    }

    T& operator[](std::size_t index) {
        return data[index];
    }

    std::size_t size() const { return total_size; }

    T* begin() { return data; }
    T* end() { return data + total_size; }
};



#endif //STATICVECTOR_H
