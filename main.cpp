#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>
#include "HashMap.h"
#include "TreeMap.h"
using namespace std;
namespace
{

template <typename K, typename V>
using Hash = aisdi::HashMap<K, V>;

template <typename K, typename V>
using Tree = aisdi::TreeMap<K, V>;

void perfomTestAppend(std::ofstream& file)
{
	for (int j=0; j<100; ++j)
	{
		Hash<int,std::string> hash;
		Tree<int,std::string> tree;
		std::chrono::nanoseconds::rep hash_aver=0;
		for (size_t i=0; i<100; ++i)
		{
			auto clock_start = std::chrono::high_resolution_clock::now();
			for (int k=0; k<j; k++)
				hash[rand()%1000]="test";
			auto clock_end = std::chrono::high_resolution_clock::now();
			hash_aver+=std::chrono::duration_cast<std::chrono::nanoseconds>(clock_end-clock_start).count();
		}
		hash_aver/=100;
		file << j << " " << hash_aver << " ";
		std::chrono::nanoseconds::rep tree_aver=0;
		for (size_t i=0; i<100; ++i)
		{
			auto clock_start = std::chrono::high_resolution_clock::now();
			for (int k=0; k<j; ++k)
				tree[rand()%1000]="test";
			auto clock_end = std::chrono::high_resolution_clock::now();
			tree_aver+=chrono::duration_cast<std::chrono::nanoseconds>(clock_end-clock_start).count();
		}
		tree_aver/=100;
		file << tree_aver << std::endl;
	}
}

void perfomTestDelete(std::ofstream& file)
{
	for (int j=0; j<100; ++j)
	{
		Hash<int,std::string> hash;
		Tree<int,std::string> tree;
		for (int i=0; i<100000; ++i)
		{
			hash[rand()%100000]="test";
			tree[rand()%100000]="test";
		}
		if (hash.getSize()<100)
			std::cout << "ups\n";
		if (tree.getSize()<100)
			std::cout << "oops\n";
		std::chrono::nanoseconds::rep hash_aver=0;
		for (size_t i=0; i<100; ++i)
		{
			auto clock_start = std::chrono::high_resolution_clock::now();
			for (int k=0; k<j; k++)
				hash.remove(hash.begin());
			auto clock_end = std::chrono::high_resolution_clock::now();
			hash_aver+=std::chrono::duration_cast<std::chrono::nanoseconds>(clock_end-clock_start).count();
		}
		hash_aver/=100;
		file << j << " " << hash_aver << " ";
		std::chrono::nanoseconds::rep tree_aver=0;
		for (size_t i=0; i<100; ++i)
		{
			auto clock_start = std::chrono::high_resolution_clock::now();
			for (int k=0; k<j; ++k)
				tree.remove(tree.begin());
			auto clock_end = std::chrono::high_resolution_clock::now();
			tree_aver+=chrono::duration_cast<std::chrono::nanoseconds>(clock_end-clock_start).count();
		}
		tree_aver/=100;
		file << tree_aver << std::endl;
	}
}

} // namespace

int main()
{
	srand(time(NULL));
  std::ofstream file;
  file.open("test_append.txt");
  file << "Test of append() function\nvector list\n";
  perfomTestAppend(file);
  file.close();
  
  
  file.open("test_prepend.txt");
  file << "Test of prepend() function\nvector list\n";
	perfomTestDelete(file);
  file.close();
  /*
  file.open("test_popFirst.txt");
  file << "Test of popFirst() function\nvector list\n";
  for (std::size_t i = 0; i < repeatCount; ++i)
    perfomTestPopFirst(file);
  file.close();
  
  file.open("test_popLast.txt");
  file << "Test of popLast() function\nvector list\n";
  for (std::size_t i = 0; i < repeatCount; ++i)
    perfomTestPopLast(file);
  file.close();*/
  
  return 0;
}
