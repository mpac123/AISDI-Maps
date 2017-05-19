#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
  
  class Node
  {
		value_type value;
		Node* left;
		Node* right;
		Node* parent;
		Node() : left(nullptr), right(nullptr), parent(nullptr) {}
		Node(const key_type key, mapped_type val, Node* parent): Node()
		{
			value=std::make_pair(key,val);
		}
		key_type getKey()
		{
			return value.first;
		}
		mapped_type getValue()
		{
			return value.second;
		}
		Node& operator=(const Node& other)
		{
			value=other.value;
			left=other->left;
			right=other->right;
			parent=other->parent;
		}
		bool operator==(const Node& other)
		{
			return value==other.value && left==other->left && right==other->right && parent==other->parent;
		}
		bool operator!=(const Node& other)
		{
			return !(*this)==other;
		}
		
	};
  
  Node* root;
  size_type size;

  TreeMap() : root(nullptr), size(0)
  {}

  ~TreeMap()
  {
		size_type s=size;
		for(int i=0; i<s; ++i)
			remove(root);
	}
	
  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it!=list.end(); ++it)
        this->operator[]((*it).first)=(*it).second;
  }

  TreeMap(const TreeMap& other) : TreeMap()
  {
    for (auto it = other.cbegin(); it != other.cend(); ++it){
        this->operator[]((*it).first) = (*it).second;
  }

  TreeMap(TreeMap&& other)
  {
    root=other.root;
    size=other.size;
    other.root=nullptr;
    other.size=0;
  }

  TreeMap& operator=(const TreeMap& other) : TreeMap()
  {
    if(this==&other)
			return *this;
		
		size_type s=size;
		for(int i=0; i<s; ++i)
			remove(root);
		
		for (auto it = other.cbegin(); it != other.cend(); ++it){
        this->operator[]((*it).first) = (*it).second;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    root=other.root;
    size=other.size;
    other.root=nullptr;
    other.size=0;
    return *this;
  }

  bool isEmpty() const
  {
    return !size;
  }

  mapped_type& operator[](const key_type& key)
  {
    Node* temp=root;
    Node* currentParent=nullptr;
    
    while(temp!=nullptr)
    {
			currentParent=temp;
			if(key==temp.getKey())
				return temp.getValue();
			else if(key>temp.getKey())
				temp=temp->right;
			else if (key<temp.getKey())
				temp=temp->left;
		}
		
		Node* newNode = new Node(key,mapped_type{},parent);
		
		if (parent!=nullptr)
		{
			if (key < parent.getKey())
				parent->left=newNode;
			else
				parent->right=newNode;
		}
		else
			root=newNode;
			
		++size;
		
		return newNode.getValue();
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp.getKey())
				return temp.getValue();
			else if(key>temp.getKey())
				temp=temp->right;
			else if (key<temp.getKey())
				temp=temp->left;
		}
		
		throw std::out_of_range("valueOf");
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp.getKey())
				return temp.getValue();
			else if(key>temp.getKey())
				temp=temp->right;
			else if (key<temp.getKey())
				temp=temp->left;
		}
		
		throw std::out_of_range("valueOf");
  }

  const_iterator find(const key_type& key) const
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp.getKey())
				return ConstIterator(temp,this);
			if(key>temp.getKey())
				temp=temp->right;
			else if (key<temp.getKey())
				temp=temp->left;
		}
		return cend();
  }

  iterator find(const key_type& key)
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp.getKey())
				return Iterator(temp,this);
			if(key>temp.getKey())
				temp=temp->right;
			else if (key<temp.getKey())
				temp=temp->left;
		}
		return end();
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    Node* temp=it.node;
    
    if (temp == nullptr || root == nullptr)
			throw std::out_of_range("remove from empty map");
		
		//no children
		if(temp->left == nullptr && temp->right == nullptr)
		{
			if (parent == nullptr)
				root=nullptr;
			else
				if (temp == parent->left)
					parent->left=nullptr;
				else if (temp == parent->right)
					parent->right=nullptr;
		}
		else if (temp->left!=nullptr and temp->right == nullptr) //only left child
		{
			if (parent == nullptr)
				root=temp->left;
			else
				if (temp == parent->left)
					parent->left=temp->left;
				else if (temp == parent->right)
					parent->right=temp->left;
		}
		else if (temp->left==nullptr and temp->right != nullptr) //only right child
		{
			if (parent == nullptr)
				root=temp->right;
			else
				if (temp == parent->left)
					parent->left=temp->right;
				else if (temp == parent->right)
					parent->right=temp->right;
		}
		else //both children
		{
			Node* replacement = temp->right;
			while(replacement->left != nullptr)
				replacement=replacement->left;
				
			temp.value=replacement.value;
			remove(replacement.getKey());
			return;
		}
		--size;
		delete temp;
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const TreeMap& other) const
  {
		if (size!=other.size)
			return false;
			
		if (root!=other.root)
			return false;
		
		return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Node* temp=root;
    if (isEmpty())
			return end();
		else
			while (temp->left != nullptr)
				temp=temp->left;
				
		return Iterator(temp, this);
  }

  iterator end()
  {
    return Iterator(nullptr, this);
  }

  const_iterator cbegin() const
  {
    Node* temp=root;
    if (isEmpty())
			return end();
		else
			while (temp->left != nullptr)
				temp=temp->left;
				
		return ConstIterator(temp, this);
  }

  const_iterator cend() const
  {
    return ConstIterator(nullptr, this);
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
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator()
  {}

  ConstIterator(const ConstIterator& other)
  {
    (void)other;
    throw std::runtime_error("TODO");
  }

  ConstIterator& operator++()
  {
    throw std::runtime_error("TODO");
  }

  ConstIterator operator++(int)
  {
    throw std::runtime_error("TODO");
  }

  ConstIterator& operator--()
  {
    throw std::runtime_error("TODO");
  }

  ConstIterator operator--(int)
  {
    throw std::runtime_error("TODO");
  }

  reference operator*() const
  {
    throw std::runtime_error("TODO");
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    (void)other;
    throw std::runtime_error("TODO");
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

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

#endif /* AISDI_MAPS_MAP_H 
