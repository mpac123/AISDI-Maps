#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  static const size_type SIZE = 20;
  std::list<value_type>* HashTable;
  
 public:
  HashMap() 
  {
    HashTable = new std::list<value_type>[SIZE]; 
  }
  
  ~HashMap()
  {
    clear();
    delete[] HashTable;
  }

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it=list.begin(); it != list.end(); ++it)
      HashTable[hashFunction((*it).first)].push_back(*it);
  }

  HashMap(const HashMap& other) : HashMap()
  {
    clear();
    for (auto it=other.begin(); it != other.end(); ++it)
        HashTable[hashFunction((*it).first)].push_back(*it);

  }

  HashMap(HashMap&& other) : HashMap()
  {
    clear();
    for (auto it=other.begin(); it != other.end(); ++it)
        HashTable[hashFunction((*it).first)].push_back(*it);
    //delete[] other.HashTable;
    //other.HashTable=nullptr;
    other.clear();
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this!=&other)
    {
      clear();
      delete[] HashTable;
      HashTable = new std::list<value_type>[SIZE]; 
      
      for (auto it=other.begin(); it != other.end(); ++it)
        HashTable[hashFunction((*it).first)].push_back(*it);
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if (this!=&other)
    {
      clear();
      delete[] HashTable;
      HashTable = new std::list<value_type>[SIZE]; 
      
      for (auto it=other.begin(); it != other.end(); ++it)
        HashTable[hashFunction((*it).first)].push_back(*it);
    }
    //delete[] other.HashTable;
    //other.HashTable=nullptr;
    other.clear();
    return *this;
  }

  bool isEmpty() const
  {
    for (size_type i=0; i<SIZE; ++i)
      if (!HashTable[i].empty())
        return false;
    return true;
  }
  
  private:
  void clear()
  {
    for(size_type i=0; i<SIZE; ++i)
      HashTable[i].clear();
  }
    
  size_type hashFunction(const key_type& key) const
  {
    return std::hash<key_type>()(key)%SIZE;
  }
    
public:
  mapped_type& operator[](const key_type& key)
  {
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
        return (*it).second;
    
    HashTable[Nr].push_back(std::make_pair(key,mapped_type{}));
    auto it=HashTable[Nr].end();
    --it;
    return (*it).second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
        return (*it).second;
    throw std::out_of_range("valueOf()");
  }

  mapped_type& valueOf(const key_type& key)
  {
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
        return (*it).second;
    throw std::out_of_range("valueOf()");
  }


  const_iterator find(const key_type& key) const
  {
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
        return ConstIterator(this, it, Nr);
    return end();
  }

  iterator find(const key_type& key)
  {
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
        return Iterator(this, it, Nr);
    return end();
  }

  void remove(const key_type& key)
  {
    if (isEmpty())
      throw std::out_of_range("remove from empty map");
    size_type Nr = hashFunction(key);
    for (auto it=HashTable[Nr].begin(); it!=HashTable[Nr].end(); ++it)
      if ((*it).first==key)
      {
        HashTable[Nr].erase(it);
        return;
      }
    throw std::out_of_range("key doesn't exist");
  }

  void remove(const const_iterator& it)
  {
    if (it==end())
      throw std::out_of_range("attempt to remove end");
    if(HashTable[it.index].empty())
      return;
    HashTable[(it.index)].erase(it.iter);
  }

  size_type getSize() const
  {
    size_type cnt=0;
    for (size_type i=0; i<SIZE; ++i)
      cnt+=HashTable[i].size();
    
    return cnt;
  }

  bool operator==(const HashMap& other) const
  {
    for(auto it = begin(), it2 = other.begin() ; it!=end(); ++it, ++it2) 
    {
      if(*it != *it2)
        return false;
        }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    for(size_type i=0; i<SIZE; ++i)
      if(!HashTable[i].empty())
        return Iterator(this,HashTable[i].begin(),i);
    return end();
  }

  iterator end()
  {
    //for(size_type i=SIZE-1; i>=0; --i)
     //if(!HashTable[i].empty())
       // return Iterator(this,--HashTable[i].end(),i);
    return Iterator(this,HashTable[SIZE].end(),SIZE);
  }

  const_iterator cbegin() const
  {
    for(size_type i=0; i<SIZE; ++i)
      if(!HashTable[i].empty())
        return ConstIterator(this,HashTable[i].begin(),i);
    return cend();
  }

  const_iterator cend() const
  {
     //   for(size_type i=SIZE-1; i>=0; --i)
     //if(!HashTable[i].empty())
       // return ConstIterator(this,--HashTable[i].end(),i);
    return ConstIterator(this,HashTable[SIZE].end(),SIZE);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
  using list_iter = typename std::list<value_type>::iterator;

private:
    const HashMap *myMap;
    list_iter iter;
    size_type index;
    
    friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);
    
public:
  explicit ConstIterator(const HashMap* my, list_iter it, size_type in) : myMap(my), iter(it), index(in)
  {}

  ConstIterator(const ConstIterator& other) : ConstIterator(other.myMap, other.iter, other.index)
  {}

  ConstIterator& operator++()
  {
    if (*this == myMap->end())
            throw std::out_of_range("out of range - operator++()");
    ++iter;
      
    if (iter == myMap->HashTable[index].end())
    {
      for (size_type i=index+1; i!=myMap->SIZE; ++i)
        if(!myMap->HashTable[i].empty())
        {
          iter=myMap->HashTable[i].begin();
          index=i;
          return *this;
        }
        *this = myMap->end();
        return *this;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if (*this == myMap->begin())
            throw std::out_of_range("out of range - operator--()");
    
    if (*this == myMap->end())
      for (size_type i=SIZE-1; i>=0; --i)
        if(!myMap->HashTable[i].empty())
        {
          iter=myMap->HashTable[i].end();
          index=i;
          --iter;
          return *this;
        }
        
    if (iter == myMap->HashTable[index].begin())
    {
      for (size_type i=index-1; i>=0; --i)
        if(!myMap->HashTable[i].empty())
        {
          iter=myMap->HashTable[i].end();
          index=i;
          --iter;
          return *this;
        }
    }
    --iter;
    return *this;
            
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  reference operator*() const
  {
    if (index==SIZE && iter==myMap->HashTable[SIZE].end())
      throw std::out_of_range("dereferencing from endIterator");
    return *iter;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return myMap == other.myMap && iter == other.iter && index==other.index;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
  using list_iter = typename std::list<value_type>::iterator;

private:
    //const HashMap *myMap;
    //size_type index;
    //list_iter iter;
    
public:
  explicit Iterator(const HashMap* my, list_iter it, size_type in) : ConstIterator(my,it,in)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
