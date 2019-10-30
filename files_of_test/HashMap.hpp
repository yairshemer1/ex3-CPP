//
// Created by Yair Shemer on 11/09/2019.
//

#ifndef EX3_HASHMAP_HPP
#define EX3_HASHMAP_HPP

static const int DIV_FACTOR = 2;

static const int EXPAND_FACTOR = 2;

static const double DEF_LOW_FACTOR = 0.25;

static const double DEF_UP_FACTOR = 0.75;

static const int DEF_CAPACITY = 16;

static const int FACTOR_BOUND = 1;

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>

/**
 * Exception to represent key failure
 */
class keyException : public std::exception
{
};

/**
 * Exception to represent bad loading factors problems
 */
class badFactorsException : public std::exception
{
};

/**
 * Exception to represent bad vectors for constructor
 */
class badVectorException : public std::exception
{
};

/**
 * Class representing open hash map data structure
 * @tparam KeyT key
 * @tparam ValueT value
 */
template<class KeyT, class ValueT>
class HashMap
{
    using bucket = typename std::vector<std::pair<KeyT, ValueT>>;
private:
    double _upperLoadFactor, _lowerLoadFactor;
    int _capacity, _elementsNum;
    bucket *_hashMap;
    std::hash<KeyT> _hashFunction;

    /**
     * Function to rehash, including resize, the data structure
     * @param newSize the new size
     */
    void _reHash(int newSize)
    {
        bucket *temp = new bucket[newSize];
        for (int i = 0; i < _capacity; i++)
        {
            for (std::pair<KeyT, ValueT> p:_hashMap[i])
            {
                temp[_hashCode(p.first, newSize)].push_back(p);
            }
        }
        _capacity = newSize;
        std::swap(_hashMap, temp);
        delete[] temp;
    }

    /**
     * Function to check if the load factors had been violated
     * @param expand is true if expand is needed, false if division
     */
    void _checkLoadFactor(bool expand)
    {
        double factor = this->getLoadFactor();
        if (factor < _lowerLoadFactor && !expand)
        {
            _reHash(_capacity / DIV_FACTOR);
        }
        else if (factor > _upperLoadFactor && expand)
        {
            _reHash(_capacity * EXPAND_FACTOR);
        }
    }

    /**
     * Function to determine the code for the table, given key and size of table
     * @param key to hash
     * @param size of table
     * @return the index
     */
    int _hashCode(KeyT key, int size) const
    {
        return _hashFunction(key) & (size - 1);
    }

public:

//    void printHash()
//    {
//        std::cout << "-----------------------------------------" << std::endl;
//        for (int i = 0; i < _capacity; i++)
//        {
//            std::cout << "index: " << i << std::endl;
//            for (auto p : _hashMap[i])
//            {
//                std::cout << " key: " << p.first << " value: " << p.second << std::endl;
//            }
//        }
//    }

    /**
     * default constructor sets the lower factor to 0.25 and upper factor to 0.75
     */
    HashMap() : HashMap(DEF_LOW_FACTOR, DEF_UP_FACTOR)
    {}

    /**
     * Constructor receiving both upper and lower factors
     * @param lowerLoadFactor of the table
     * @param upperLoadFactor of the table
     * @exception if factors are not valid (between 0 and 1)
     */
    HashMap(double lowerLoadFactor, double upperLoadFactor) :
            _upperLoadFactor(upperLoadFactor),
            _lowerLoadFactor(lowerLoadFactor),
            _capacity(DEF_CAPACITY),
            _elementsNum(0),
            _hashMap(new bucket[_capacity])
    {
        if (lowerLoadFactor > upperLoadFactor || lowerLoadFactor < 0 || upperLoadFactor < 0 ||
            lowerLoadFactor > FACTOR_BOUND || upperLoadFactor > FACTOR_BOUND)
        {
            delete[] _hashMap;
            throw badFactorsException();
        }
    }

    /**
     * Receiving two vectors of keys and values, this constructor sets the map through that
     * @param v1 keys vector
     * @param v2 values vector
     * @exception if the vectors are not same size
     */
    HashMap(std::vector<KeyT> v1, std::vector<ValueT> v2) : HashMap()
    {
        if (v1.size() != v2.size())
        {
            throw badVectorException();
        }
        else
        {
            for (int i = 0; i < v1.size(); i++)
            {
                (*this)[v1[i]] = v2[i];
            }
        }
    }

    /**
     * Copy constructor
     * @param other hash map to copy from
     */
    HashMap(const HashMap &other) :
            _capacity(other._capacity),
            _elementsNum(other._elementsNum),
            _upperLoadFactor(other._upperLoadFactor),
            _lowerLoadFactor(other._lowerLoadFactor),
            _hashMap(new bucket[_capacity])
    {
        for (int i = 0; i < _capacity; i++)
        {
            bucket newBucket(other._hashMap[i]);
            _hashMap[i] = newBucket;
        }
    }

    /**
     * Move constructor
     * @param other hash map as rvalue
     */
    HashMap(HashMap && other) noexcept:
            _capacity(other._capacity),
            _elementsNum(other._elementsNum),
            _upperLoadFactor(other._upperLoadFactor),
            _lowerLoadFactor(other._lowerLoadFactor),
            _hashMap(other._hashMap)
    {
        other._hashMap = nullptr;
    }

    /**
     * Destructor
     */
    ~HashMap()
    {
        delete[] _hashMap;
    }

    /**
     * Number of elements in the array
     * @return the size of the map
     */
    int size() const
    {
        return _elementsNum;
    }

    /**
     * Capacity is possible size of table
     * @return the capacity
     */
    int capacity() const
    {
        return _capacity;
    }

    /**
     * Current load factor
     * @return the current load factor
     */
    double getLoadFactor() const
    {
        return (double) _elementsNum / (double) _capacity;
    }

    /**
     * the emptiness of the table
     * @return true if the table is empty, false otherwise
     */
    bool empty() const
    {
        return _elementsNum == 0;
    }

    /**
     * A function to insert a new pair to the table
     * @param key of the new element
     * @param val of the new element
     * @return true upon success, false otherwise
     */
    bool insert(const KeyT key, const ValueT val)
    {
        if (containsKey(key))
        {
            return false;
        }
        try
        {
            _elementsNum++;
            _checkLoadFactor(true);
            _hashMap[_hashCode(key, _capacity)].push_back(std::pair<KeyT, ValueT>(key, val));
            return true;
        }
        catch (std::bad_alloc &)
        {
            _elementsNum--;
            return false;
        }
    }

    /**
     * If the table contains a key
     * @param key to check
     * @return true if found, false otherwise
     */
    bool containsKey(const KeyT key) const
    {
        for (std::pair<KeyT, ValueT> p: _hashMap[_hashCode(key, _capacity)])
        {
            if (p.first == key)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * A function to check what is at the given key
     * @param key to search
     * @return the value binded to the key
     * @exception if the key is not found
     */
    ValueT &at(const KeyT &key) const
    {
        for (std::pair<KeyT, ValueT> &p: _hashMap[_hashCode(key, _capacity)])
        {
            if (p.first == key)
            {
                return p.second;
            }
        }
        throw keyException();
    }

    /**
     * Function to erase a key, if located
     * @param key to delete
     * @return true upon success, false otherwise
     */
    bool erase(const KeyT key)
    {
        if (!containsKey(key))
        {
            return false;
        }
        try
        {
            _elementsNum--;
            _checkLoadFactor(false);
        }
        catch (std::bad_alloc &)
        {
            _elementsNum++;
            return false;
        }
        //find the key
        auto it = _hashMap[_hashCode(key, _capacity)].begin();
        while (it->first != key)
        {
            it++;
        }
        _hashMap[_hashCode(key, _capacity)].erase(it);
        return true;
    }

    /**
     * Size of bucket
     * @param key to search
     * @return the size of the bucket they key belongs to
     * @exception if the key is bind to an empty cell
     */
    int bucketSize(const KeyT key) const
    {
        if (_hashMap[_hashCode(key, _capacity)].empty())
        {
            throw keyException();
        }
        return _hashMap[_hashCode(key, _capacity)].size();
    }

    /**
     * Clear the table
     */
    void clear()
    {
        delete[] _hashMap;
        _elementsNum = 0;
        _hashMap = new bucket[_capacity];
    }
    
    /**
     * Assignment operator
     * @param other hashmap to assign
     * @return the assigned hash map
     */
    HashMap &operator=(const HashMap &other)
    {
        _capacity = other._capacity;
        _elementsNum = other._elementsNum;
        _upperLoadFactor = other._upperLoadFactor;
        _lowerLoadFactor = other._lowerLoadFactor;
        for (int i = 0; i < _capacity; i++)
        {
            bucket newBucket(other._hashMap[i]);
            _hashMap[i] = newBucket;
        }
        return *this;
    }

    /**
     * Move assignment operator
     * @param other other a hash map to assign as rvalue
     * @return reference to the assigned hash map
     */
    HashMap &operator=(HashMap && other) noexcept
    {
        _capacity = other._capacity;
        _elementsNum = other._elementsNum;
        _upperLoadFactor = other._upperLoadFactor;
        _lowerLoadFactor = other._lowerLoadFactor;
        for (int i = 0; i < _capacity; i++)
        {
            bucket newBucket(other._hashMap[i]);
            _hashMap[i] = newBucket;
        }
        //reset other
        delete[] other._hashMap;
        other._hashMap = nullptr;
        other._capacity = 0;
        other._elementsNum = 0;
        other._upperLoadFactor = 0;
        other._lowerLoadFactor = 0;
        return *this;
    }

    /**
     * Write operator[]
     * @param key to reach
     * @return the access to the value
     */
    ValueT &operator[](const KeyT key)
    {
        try
        {
            return at(key);
        }
        catch (keyException &)
        {
            insert(key, ValueT());
            return at(key);
        }
    }

    /**
     * Read operator []
     * @param key to reach
     * @return a const reference to the map
     */
    const ValueT &operator[](const KeyT key) const
    {
        try
        {
            return at(key);
        }
        catch (keyException &)
        {
        }
    }

    /**
     * Equality operator
     * @param other hash map to compare with
     * @return true if equal, false otherwise
     */
    bool operator==(const HashMap &other) const
    {
        if (_lowerLoadFactor != other._lowerLoadFactor || _elementsNum != other._elementsNum ||
            _upperLoadFactor != other._upperLoadFactor || _capacity != other._capacity)
        {
            return false;
        }
        for (int i = 0; i < _capacity; i++)
        {
            for (std::pair<KeyT, ValueT> p:_hashMap[i])
            {
                if (std::find(other._hashMap[i].begin(), other._hashMap[i].end(), p) ==
                    other._hashMap[i].end())
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Inequality operator
     * @param other hash map to compare with
     * @return true if not equal, false otherwise
     */
    bool operator!=(const HashMap &other) const
    {
        return !(*this == other);
    }

//    class iterator
//    {
//    private:
//        int _bucketIndex, _vecIndex;
//        HashMap *_map;
//    public:
//
//        iterator(HashMap *hashMap, int row = 0, int col = 0) :
//                _map(hashMap),
//                _bucketIndex(row),
//                _vecIndex(col)
//        {
//            while (_map->_hashMap[_bucketIndex].empty())
//            {
//                _bucketIndex++;
//            }
//        }
//
//        iterator &operator++()
//        {
//            if (_vecIndex == _map->_hashMap[_bucketIndex].size() - 1)
//            {
//                _bucketIndex++;
//                _vecIndex = 0;
//                while (_map->_hashMap[_bucketIndex].empty() && _bucketIndex != _map->_capacity)
//                {
//                    _bucketIndex++;
//                }
//                return *this;
//            }
//            else
//            {
//                _vecIndex++;
//            }
//            return *this;
//        }
//
//        iterator operator++(int)
//        {
//            iterator temp = *this;
//            *this++;
//            return temp;
//        }
//
//        std::pair<KeyT, ValueT> &operator*() const
//        {
//            return _map->_hashMap[_bucketIndex].at(_vecIndex);
//        }
//
//        std::pair<KeyT, ValueT> *operator->() const
//        {
//            return &(_map->_hashMap[_bucketIndex].at(_vecIndex));
//        }
//
//        bool operator==(iterator const &other) const
//        {
//            return other._map == _map && other._vecIndex == _vecIndex &&
//                   other._bucketIndex == _bucketIndex;
//        }
//
//        bool operator!=(iterator const &other) const
//        {
//            return !(*this == other);
//        }
//    };
    /**
     * Class that enables iterating over the map, where it stays constant
     */
    class const_iterator
    {
    private:
        const HashMap *_map;
        int _bucketIndex, _vecIndex;
    public:
        /**
         * Constructor of the iterator, sets the iteration to the first one
         * @param hashMap pointer of hashmap
         * @param row has default value of 0
         * @param col has default value of 0
         */
        const_iterator(const HashMap<KeyT, ValueT> *hashMap, int row = 0, int col = 0) :
                _map(hashMap),
                _bucketIndex(row),
                _vecIndex(col)
        {
            while (_map->_hashMap[_bucketIndex].empty())
            {
                _bucketIndex++;
            }
        }

        /**
         * Increment prefix operator
         * @return const reference to the next iterator
         */
        const const_iterator &operator++()
        {
            if (_vecIndex == (int) _map->_hashMap[_bucketIndex].size() - 1)
            {
                _bucketIndex++;
                _vecIndex = 0;
                while (_map->_hashMap[_bucketIndex].empty() && _bucketIndex != _map->_capacity)
                {
                    _bucketIndex++;
                }
                return *this;
            }
            else
            {
                _vecIndex++;
            }
            return *this;
        }

        /**
         * Increment postfix operator
         * @return value of the next iterator
         */
        const_iterator operator++(int)
        {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }

        /**
         * Dereference operator
         * @return read access to the pair
         */
        const std::pair<KeyT, ValueT> &operator*() const
        {
            return _map->_hashMap[_bucketIndex].at(_vecIndex);
        }

        /**
         * Access operator
         * @return read access to the pair
         */
        const std::pair<KeyT, ValueT> *operator->() const
        {
            return &(_map->_hashMap[_bucketIndex].at(_vecIndex));
        }

        /**
         * Equality operator
         * @param other iterator to compare
         * @return true if equal, false otherwise
         */
        bool operator==(const_iterator const &other) const
        {
            return other._map == _map && other._vecIndex == _vecIndex &&
                   other._bucketIndex == _bucketIndex;
        }

        /**
         * Inequality operator
         * @param other iterator to compare
         * @return true if inequal, false otherwise
         */
        bool operator!=(const_iterator const &other) const
        {
            return !(*this == other);
        }
    };

    /**
     * First iterator of the hash map
     * @return the iterator of the beginning of the map
     */
    const_iterator begin() const
    {
        return const_iterator(this);
    }

    /**
     * Last iterator of the hash map
     * @return the iterator of the end of the map
     */
    const_iterator end() const
    {
        return const_iterator(this, _capacity, 0);
    }

    /**
     * First iterator of the hash map
     * @return the iterator of the beginning of the map
     */
    const_iterator cbegin() const
    {
        return const_iterator(this);
    }

    /**
     * Last iterator of the hash map
     * @return the iterator of the end of the map
     */
    const_iterator cend() const
    {
        return const_iterator(this, _capacity, 0);
    }
};

#endif //EX3_HASHMAP_HPP
