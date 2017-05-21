#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>

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
		public:
		value_type* value;
		Node* left;
		Node* right;
		Node* parent;
		Node() : left(nullptr), right(nullptr), parent(nullptr), value(nullptr) {}
		Node(value_type val, Node* parent): left(nullptr), right(nullptr), parent(parent)
		{
			value=new value_type(val);
			//Node::parent=parent;
		}
		~Node()
		{
			if (value)
				delete value;
		}
		key_type getKey()
		{
			return value->first;
		}
		mapped_type& getValue()
		{
			return value->second;
		}
		value_type& getPair()
		{
			return *value;
		}
		Node& operator=(const Node& other)
		{
			value=other->value;
			left=other->left;
			right=other->right;
			parent=other->parent;
		}
		bool operator==(const Node& other)
		{
			return value==other->value && left==other->left && right==other->right && parent==other->parent;
		}
		bool operator!=(const Node& other)
		{
			return !((*this)==other);
		}
		
	};
  
  Node* root;
  size_type size;

  TreeMap() : root(nullptr), size(0)
  {}

  ~TreeMap()
  {
		size_type s=size;
		for(size_type i=0; i<s; ++i)
			remove(root->getKey());
	}
	
  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it!=list.end(); ++it)
        this->operator[]((*it).first)=(*it).second;
  }

  TreeMap(const TreeMap& other) : TreeMap()
  {
    for (auto it = other.cbegin(); it != other.cend(); ++it)
        this->operator[]((*it).first) = (*it).second;
  }

  TreeMap(TreeMap&& other) : TreeMap()
  {
    root=other.root;
    size=other.size;
    other.root=nullptr;
    other.size=0;
  }

  TreeMap& operator=(const TreeMap& other) 
  {
    if(this==&other)
			return *this;
		
		size_type s=size;
		for(size_type i=0; i<s; ++i)
			remove(root->getKey());
		
		for (auto it = other.cbegin(); it != other.cend(); ++it)
        this->operator[]((*it).first) = (*it).second;
        
    return *this;
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
			if(key==temp->getKey())
				return temp->getValue();
			else if(key>temp->getKey())
				temp=temp->right;
			else if (key<temp->getKey())
				temp=temp->left;
		}
		
		Node* newNode = new Node(std::make_pair(key,mapped_type{}),currentParent);
		++size;
		
		if (currentParent!=nullptr)
		{
			if (key < currentParent->getKey())
				currentParent->left=newNode;
			else
				currentParent->right=newNode;
		}
		else
			root=newNode;
			
		
		
		return newNode->getValue();
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp->getKey())
				return temp->getValue();
			else if(key>temp->getKey())
				temp=temp->right;
			else if (key<temp->getKey())
				temp=temp->left;
		}
		
		throw std::out_of_range("valueOf");
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp->getKey())
				return temp->getValue();
			else if(key>temp->getKey())
				temp=temp->right;
			else if (key<temp->getKey())
				temp=temp->left;
		}
		
		throw std::out_of_range("valueOf");
  }

  const_iterator find(const key_type& key) const
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp->getKey())
				return ConstIterator(temp,this);
			if(key>temp->getKey())
				temp=temp->right;
			else if (key<temp->getKey())
				temp=temp->left;
		}
		return cend();
  }

  iterator find(const key_type& key)
  {
    Node* temp=root;
    
    while(temp!=nullptr)
    {
			if(key==temp->getKey())
				return Iterator(temp,this);
			if(key>temp->getKey())
				temp=temp->right;
			else if (key<temp->getKey())
				temp=temp->left;
		}
		return end();
  }

  void remove(const key_type& key)
  {
		key_type k=key;
    remove(find(k));
  }

  void remove(const const_iterator& it)
  {
		remove(it.node);
	}
	
  void remove(Node* temp)
  {
    //Node* temp=it.node;
    
    if (temp == nullptr)
			throw std::out_of_range("iterator is null");
		
		   if (root == nullptr)
		throw std::out_of_range("remove from empty map");
			
		//no children
		if(temp->left == nullptr && temp->right == nullptr)
		{
			if (temp->parent == nullptr)
				root=nullptr;
			else
				if (temp == temp->parent->left)
					temp->parent->left=nullptr;
				else if (temp == temp->parent->right)
					temp->parent->right=nullptr;
		}
		else if (temp->left!=nullptr and temp->right == nullptr) //only left child
		{
			if (temp->parent == nullptr)
			{
				root=temp->left;
				temp->left->parent=nullptr;
			}
			else
				if (temp == temp->parent->left)
				{
					temp->parent->left=temp->left;
					temp->left->parent=temp->parent;
				}
				else if (temp == temp->parent->right)
				{
					temp->parent->right=temp->left;
					temp->left->parent=temp->parent;
				}
				else 
					throw std::out_of_range("ooops");
		}
		else if (temp->left==nullptr and temp->right != nullptr) //only right child
		{
			if (temp->parent == nullptr)
			{
				root=temp->right;
				temp->right->parent=nullptr;
			}
			else
				if (temp == temp->parent->left)
				{
					temp->parent->left=temp->right;
					temp->right->parent=temp->parent;
				}
				else if (temp == temp->parent->right)
				{
					temp->parent->right=temp->right;
					temp->right->parent=temp->parent;
				}
				else 
					throw std::out_of_range("ooops");
		}
		else //both children
		{
			Node* replacement = temp->right;
			
			while(replacement->left != nullptr)
				replacement=replacement->left;
				
			value_type* val;
			val = new value_type(replacement->value->first,replacement->value->second);
			
			delete temp->value;
			temp->value=val;
			
			if (temp->value == temp->right->value)
				temp->right=nullptr;
			
			remove(replacement);
			
			return;
			/*replacement->parent=temp->parent;
			if (replacement->right != temp->right)
				replacement->right=temp->right;
			else
				replacement->right=nullptr;
			replacement->left=temp->left;
			temp->left->parent=replacement;
			temp->right->parent=replacement;
			if (temp != root)
			{
				if (temp==temp->parent->left)
					temp->parent->left=replacement;
				else if (temp==temp->parent->right)
					temp->parent->right=replacement;
			}*/
			
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
			
		for (auto it1=begin(), it2=other.begin(); it1!=end() || it2!=other.end(); ++it1, ++it2)
			if ((*it1).first != (*it2).first || (*it1).second != (*it2).second)
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
  
  const TreeMap *map;
  Node *node;

  explicit ConstIterator() : map(nullptr), node(nullptr)
  {}

	ConstIterator(Node* n, const TreeMap* m) : map(m), node(n)
	{}
	
  ConstIterator(const ConstIterator& other) : map(other.map), node(other.node)
  {}

  ConstIterator& operator++()
  {
    if (node==nullptr)
			throw std::out_of_range("increasing end()");
		
		if(node->right != nullptr)
		{
			node=node->right;
			while(node->left != nullptr)
				node=node->left;
		}
		else
		{
			while(node->parent != nullptr && node->parent->right==node)
				node=node->parent;
			if(node->parent != nullptr)
				node=node->parent;
			else
				node=nullptr;
		}
		return *this;
  }

  ConstIterator operator++(int)
  {
    auto result=*this;
    operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if (*this==map->begin() || map->size <= 0)
			throw std::out_of_range("decreasing begin or empty map");
		if (node == nullptr)
		{
			Node* temp=map->root;
			
			while(temp->right!=nullptr)
				temp=temp->right;
			
			node=temp;
			return *this;
		}
		else if (node->left != nullptr)
		{
			node=node->left;
			while(node->right!=nullptr)
				node=node->right;
		}
		else
		{
			while(node->parent!=nullptr && node->parent->left == node)
				node=node->parent;
				
			if (node->parent != nullptr)
				node=node->parent;
			else
				node=nullptr;
		}
		return *this;
  }

  ConstIterator operator--(int)
  {
    auto result=*this;
    operator--();
    return result;
  }

  reference operator*() const
  {
		if (*this==map->end())	
			throw std::out_of_range("reference to end()");
    return node->getPair();
  }

  pointer operator->() const
  {
		if (*this==map->end())	
			throw std::out_of_range("pointer to end()");
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return node==other.node && map==other.map;
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

  explicit Iterator() : ConstIterator()
  {}

	Iterator(Node* n, const TreeMap* m) : ConstIterator(n,m)
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

#endif //AISDI_MAPS_MAP_H 
