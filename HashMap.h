#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

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
  //struktura przechowujaca liste elementow o tym samym kluczu mieszajacym
  struct HashNode
  {
		value_type data;
		HashNode* next;
		HashNode* prev;
		//konstruktor stosowany, gdy jeszcze zaden element nie znajduje sie w tablicy pod tym kluczem
		HashNode(key_type key, mapped_type mapped) : data(std::make_pair(key, mapped)), next(nullptr), prev(nullptr) {}
		//konstruktor stosowany, gdy jakis element znajduje sie juz pod danym kluczem w tablicy;
		//nowt element zostaje dodany na koniec listy HashNodów o tym samym kluczu
		HashNode(key_type key, mapped_type mapped, HashNode* prev) : HashNode(key, mapped)
		{
			this->prev = prev;
		}
		~HashNode() {}
	};
	HashNode **tab;
	size_type cnt;
  
 public:
  HashMap() : tab(nullptr), cnt(0)
  {
		tab = new HashNode* [SIZE]{nullptr};
	}
	
	~HashMap()
	{
		erase();
		delete[] tab;
	}

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it=list.begin(); it != list.end(); ++it)
    {
		
		
		operator[]((*it).first)=(*it).second;
	}
  }

  HashMap(const HashMap& other)
  {
    *this=other;
  }

  HashMap(HashMap&& other)
  {
    *this=std::move(other);
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this!=&other)
    {
			erase();
			for (auto it=other.begin(); it!= other.end(); ++it)
				operator[]((*it).first)=(*it).second;
		}
		return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if (this!=&other)
    {
			erase();
			tab=other.tab;
			cnt=other.cnt;
			other.tab=nullptr;
			other.cnt=0;
		}
		return *this;
  }

  bool isEmpty() const
  {
    return cnt==0;
  }
  
  private:
  void erase()
  {
		for(size_type i=0; i<SIZE; ++i)
		{
			delete tab[i];
			tab[i]=nullptr;
		}
		cnt=0;
	}
	
	size_type hashFunction(const key_type& key) const
	{
		return std::hash<key_type>()(key)%SIZE;
	}
	
public:
  mapped_type& operator[](const key_type& key)
  {
	return valueOf(key);
  }
  
 private:
	HashNode* findNode(const key_type& key) const
	{
		size_type nr=hashFunction(key);
    HashNode* node = tab[nr];
    
    if(node == nullptr) 
			return nullptr;
    else if(node->data.first != key)
    {
				do
				{
					if(node->next == nullptr)
						return nullptr;
					node=node->next;
				}while(node->data.first!=key);
		}
		else
			node=node->next;
			
		return node;
	}
	
 public:

  const mapped_type& valueOf(const key_type& key) const
  {
    HashNode* node = findNode(key);
    if(node==nullptr)
			throw std::out_of_range("out of range - valueOf()");
		return node->data.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    HashNode* node = findNode(key);
    if(node==nullptr)
			throw std::out_of_range("out of range - valueOf()");
		return node->data.second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, findNode(key), hashFunction(key));
  }

  iterator find(const key_type& key)
  {
    return iterator(this, findNode(key), hashFunction(key));
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(it.myNode==nullptr)
			throw std::out_of_range("out of range - remove()");
		
		auto node=it.myNode;
		if(node->prev == nullptr) //pierwszy element listy w danej komorce tabeli
			tab[hashFunction(node->data.first)]=node->next;
		else
			node->prev->next=node->next;
			
		if(node->next != nullptr)
			node->next->prev = node->prev;
			
		node->next = nullptr;
		delete node;
		--cnt;
  }

  size_type getSize() const
  {
    return cnt;
  }

  bool operator==(const HashMap& other) const
  {
    if(cnt != other.cnt)
      return false;

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
    size_type index=0;
    while(tab[index]==nullptr and index<SIZE)
			++index;
		return Iterator(this,tab[index],index);
  }

  iterator end()
  {
    return Iterator(this,nullptr,SIZE);
  }

  const_iterator cbegin() const
  {
    size_type index=0;
    while(tab[index]==nullptr and index<SIZE)
			++index;
		return ConstIterator(this,tab[index],index);
  }

  const_iterator cend() const
  {
    return ConstIterator(this,nullptr,SIZE);
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

private:
	const HashMap *myMap;
	HashNode *myNode;
	size_type index;
	friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);
	
public:
  explicit ConstIterator(const HashMap* mym=nullptr, HashNode* myn=nullptr, size_type in=0) : myMap(mym), myNode(myn), index(in)
  {
		if (myNode==nullptr && myMap != nullptr)
			this->index=myMap->SIZE; //jestesmy na koncu
	}

  ConstIterator(const ConstIterator& other) : ConstIterator(other.myMap, other.myNode, other.index)
  {}

  ConstIterator& operator++()
  {
    if(myMap == nullptr or myNode == nullptr)
			throw std::out_of_range("out of range - operator++()");
		else if(myNode->next != nullptr)
			myNode=myNode->next; //jestesmy w tej samej "galezi"
		else //koniec galezi, przechodzimy do kolejnej komorki tablicy
		{
			do
			{
				++index;
				if (index >= myMap->SIZE)
				{
					myNode = nullptr;
					return *this;
				}
			}while(myMap->table[index]==nullptr);
		}
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if (myMap==nullptr)
			throw std::out_of_range("out of range - operator--()");
		else if (myNode == nullptr or myNode == myMap->tab[index]) //czyli jestesmy pierwszym elementem w liscie danej komorki tabeli
		{
			do
				--index;
			while(index>0 and myMap->tab[index] == nullptr);
			
			if (index==0 and myMap->table[index]==nullptr)
				throw std::out_of_range("out of range - operator--()");
			
			myNode = myMap->table[index];
			while (myNode->next != nullptr)
				myNode = myNode->next;
		}
		else
			myNode = myNode->prev; //przed nami jest jakis element na liscie
			
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
    if(myNode==nullptr)
			throw std::out_of_range("out of range - operator*()");
		return myNode->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return myMap == other.myMap && myNode == other.myNode && index==other.index;
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


  explicit Iterator()
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
