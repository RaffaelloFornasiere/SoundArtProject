#ifndef M_VECTOR_H
#define M_VECTOR_H
#include <Arduino.h>



template<class Q>
class MVectorIterator;


template <class T>
class MVector
{
private:

    size_t _size;
    size_t cap;
    T* data;

    void _setCap(size_t cap)
    {
        T* aux = new T[cap];
        this->cap = cap;
        for(size_t i = 0; i < _size; i++)
            aux[i] = data[i];
        delete[] data;
        data = aux;
    }



public:
    MVector() : _size(0), cap(0), data(nullptr) {}
    MVector(size_t size): _size(size), cap (size), data(new T[size]) {}
    ~MVector(){delete[] data;}

    T& operator[](size_t pos) {return data[pos];}

    T& at(size_t pos)
    {
        if(pos < _size)
            return data[pos];
#ifdef MVECTOR_EXCEPTION
        Serial.println("*******MVECTOR_ERROR**********");
        Serial.println("out_of_range");
        Serial.println("******************************")
#endif
    }

    void push_back(T elem)
    {
        if(_size + 1 > cap)
        {
            if(data == nullptr)
                data = new T[1];
            _setCap(cap?cap*2:2);
        }
        _size++;
        data[_size-1] = elem;
    }
    size_t size() {return _size;}
    void insert(T elem, size_t pos)
    {
        if(_size + 1 > cap)
            _setCap(cap?2*cap:2);

        _size++;
        for(size_t i = _size; i > pos; i--)
            data[pos] = data[pos-1];

        data[pos] = elem;
    }
    void erase(size_t pos)
    {
        if(pos >= _size)
            return;
        for(size_t i = pos; i < _size-1; i++)
            data[i] = data[i+1];
        _size--;
    }
    void reserve(size_t cap)
    {
        if(cap <= this->cap)
            return;
        this->cap = cap;
        _setCap(cap);
    }
    void resize(size_t size)
    {
        if(_size > cap)
            reserve(_size);
        this->_size = size;
    }
    void shrink_to_fit()
    {
        T* aux = new T[_size];
        for(size_t i = 0; i < _size; i++)
            aux[i] = data[i];
        delete[] data;
        data = aux;
    }



    MVectorIterator<T> begin() {return MVectorIterator<T>(*this, 0);}
    MVectorIterator<T> end() {return MVectorIterator<T>(*this, this->_size);}
};





template<class Q>
class MVectorIterator
{
public:
    MVectorIterator(MVector<Q>& collection, size_t index)
        : index(index), collection(collection){}

    bool operator != (const MVectorIterator& other) const {return index != other.index;}
    Q& operator*() const {return collection.at(index);}
    const MVectorIterator& operator++() { ++index; return *this;}

private:
    size_t index;
    MVector<Q>& collection;
};



#endif
