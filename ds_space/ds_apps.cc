 
#include <vector> 
#include <ostream>
#include <algorithm> 

#include <gtest/gtest.h>
 
using std::vector;

template <typename T>
class HeapEasy {
public:
  // удаляет элемент - тут сразу проблема как со стеком
  // setter не должен возвращать по значению
  //T extract_min(); -> 
  T top() const;
  void pop();
  
  void insert(const T& val);
  
  void heapify(const vector<T>& unordered_data);
  
  // Вообще как это сделать? - нужно найти позицию
  // http://stackoverflow.com/questions/2372994/search-an-element-in-a-heap
  //void delete_elem(const T& val);  // кажется нужно для алгоритма Дейкстры
  
  // Trouble
  // http://stackoverflow.com/questions/649640/how-to-do-an-efficient-priority-update-in-stl-priority-queue
  // http://stackoverflow.com/questions/14016189/does-changing-a-priority-queue-element-result-in-resorting-the-queue
  //void increase_decrease_key(const T& key);
public:
  
};

TEST(DS, HeapV0) {
  HeapEasy<int> e;
  int arr[] = {1, 4, 7, 3, 7, 9, 2, 9};
  vector<int> in(arr, arr+sizeof arr / sizeof arr[0]);
}

  
/*
// похоже можно пользоваться реализацией кучи из std::
vector<int> v(array, array+5);

for_each(begin(v), end(v), [] (const int& elem) { cout << elem << " "; });
cout << endl;

// http://stackoverflow.com/questions/7681779/easy-way-to-maintain-a-min-heap-with-stl
// по умолчанию less, и выходит max-heap
make_heap(begin(v), end(v), greater<int>());  // min heap
cout << v.front() << endl;  // TODO: top() - это первый элемент?

for_each(begin(v), end(v), [] (const int& elem) { cout << elem << " "; });
cout << endl;

// http://stackoverflow.com/questions/14016921/comparator-for-min-heap-in-c
// Где вершина то?
// Похоже нужно использовать один и тот же компаратор
*/

// Похоже для алгоритма Дектсры лучше bst 

class UnionFind {};

class LinkedList {};

// TODO: Boost.Thread - message query 

// TODO: конкурентные очереди из нового стандарта

// Stack от IBM - pthread -> boost
class ConcurentStack {};

// Конкурентная очередь от Шена Парента - она не очень, но все же

// TODO: изучить heap из opencv
