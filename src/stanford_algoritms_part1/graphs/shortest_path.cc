// Type converting:
// "if you use Boost.Tokenizer, why not to replace atoi by boost::lexical_cast?"
//
// String processing: 
//   http://www.codeproject.com/Articles/23198/C-String-Toolkit-StrTk-Tokenizer
//
// STL alg.L:
// http://stackoverflow.com/questions/11343116/rotating-a-vector-array
//
// TODO:
// shared_ptr< const V > - нужно для пероизовдительности при работе алгоритмов - но это обращение к куче!
// Own ref. counting - 
//
// OpenCV - ass Wrapper, ReferenceHandler, refcount in core_c.h, refcount->, int* refcount;, int refcount
//   struct PtrOwner - ptr.inl.hpp, struct ImplMutex::Impl
//   Mat-objects - ?
// Hypertable: ReferenceCount, intrusive_ptr
//
// Summary:
// - используют как базовый
// - можно как-то работать не только с классами, но и со структурами - см. OpenCV
// OpenCV: //! Smart pointer for OpenGL 2D texture memory with reference counting. - 

// C
#include <cassert>

// C++
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#include <limits>       // std::numeric_limits

// http://www.onlamp.com/pub/a/onlamp/2006/04/06/boostregex.html?page=3
//#include <boost/regex.hpp>  // too hard
//#include "boost/lexical_cast.hpp"  // не то, что нужно - это уже для строк со снятым форматированием

#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <boost/foreach.hpp>

#include "shortest_path.h"

using namespace std;
using namespace tbb;

#define foreach BOOST_FOREACH

Neighbor::Neighbor(const EdgeMaker& maker) {
  // TODO: посмотреть в Саттере где два объект значения
  // No exception safe - но кажется и нельзя сделат безопасным
  // Типы базовые, поэтому таки безопасно
  weight = maker.weight_;
  end = maker.end_;
}

Neighbor EdgeMaker::create() {
  Neighbor edge;
  edge.end = end_;
  edge.weight = weight_;
  return edge;    
}

namespace graph_persistency {
typedef vector<Neighbor> Neighbors;  // заменить на СОСЕДЕЙ
//typedef pair<int, Neighbors> Neighbors; 
//typedef Neighbors Neighbors; 

/*
TEST: 
  string test_line("0\t8,89\t9,90\t\n");
  stringstream ss;
  pair<int, Neighbors> test = parse_node_data(test_line, ss);
  assert(test.second.size() == 2);
 */
// TODO: Возвращаемое значение не всегда копируется?
pair<int, Neighbors> parse_node_data(const string& line, stringstream& ss) 
{
  // 0\t8,89\t...  source sink, weight ... -> 0,8,89 - 
  const char kSplitter = ',';
  
  string line_copy = line;  // TODO: slow
  
  // trim
  // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
  line_copy.erase(line_copy.find_last_not_of(" \n\r\t")+1);
  
  // http://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
  // возможно можно исключить
  replace(line_copy.begin(), line_copy.end(), '\t', kSplitter);  // заменяем символы, а не строки

  // можно разбивать на части
  // stringstream - http://habrahabr.ru/post/131977/
  // http://stackoverflow.com/questions/1894886/parsing-a-comma-delimited-stdstring 
  // Try reload
  ss.str(line_copy); 
  ss.clear();
  
  // main node
  int root = 0;
  ss >> root;  // похоже довольно интеллектуальная операция
  if (ss.peek() == kSplitter)
      ss.ignore();

  vector<Neighbor> result;  // TODO: to deque
  result.reserve(100);  // защита от лишний аллокаций 
  int max_node_idx = 0;
  while (true) 
  {
    int i = 0;
    bool v = ss >> i;
    if (!v)
      break;
    if (ss.peek() == kSplitter)
      ss.ignore();
    
    // Find max
    if (i > max_node_idx)
      max_node_idx = i;
    
    // W
    int j = 0;
    bool w = ss >> j;
    if (!w)
      throw invalid_argument("Error: String format is broken.");
    if (ss.peek() == kSplitter)
      ss.ignore();
    
    result.push_back(EdgeMaker().end(i).weight(j));
  }

  //make_pair(max_node_idx
  return make_pair(root, result); // сразу не поставил а gcc не отловил - в итоге дамп памяти
}

class RawYieldFunctor {
public:
  RawYieldFunctor() {}
  
  //@Stateless
  pair<int, Neighbors> operator()(const string& arg) const {
    // Передать по ссылке или вернуть значение?
    // Самый быстрый вариант! Нет в цикле есть IO-операции - так что измерения не очень честные.
    // Если вне цикла, то долго. И если передавать в функцию, то тоже долго, но чуть меньше.
    // Нет, кажется не принципиально
    stringstream ss;  // он тяжелый!!! но как его сбросить?
    pair<int, Neighbors> raw_code = parse_node_data(arg, ss);
    //assert(raw_code.size() > 1);
    return raw_code;
  }
  
private:
  // не копируемый похоже
  //stringstream g_ss;  // он тяжелый!!! но как его сбросить?
};

class ApplyFoo {  
  const string* const array; // map only!!
  pair<int, Neighbors>* const out;
  const RawYieldFunctor op;
public:  
  ApplyFoo (const string* a, pair<int, Neighbors>* out_a): array(a), out(out_a) {}  
  void operator()( const blocked_range<int>& r ) const {  
      for (int i = r.begin(), end = r.end(); i != end; ++i ) { 
	// TODO: i - глобальный индек или нет?
	out[i] = op(array[i]);  
      }  
  }  
};

vector<string> extract_records(const string& filename) 
{
  fstream stream(filename.c_str());
  if (!stream)
    throw runtime_error("Error: can't open file");

  vector<string> records;
  // IO operations
  { 
    records.reserve(200);
    string line;  // и не видна, и не в цикле
    while (true) {
      // можно и в буффер читать, но так показалось что проще завершить чтение
      if (!std::getline(stream, line))  
	break;
      records.push_back(line);
    }
  }
  return records;  
}

vector<pair<int, Neighbors> > process_parallel(const vector<string>& records) {
  // нужно реально выделить, резервирование не подходит
  vector<pair<int, Neighbors> > raw_nodes(records.size());  

  // No speed up
  // Linux CPU statistic.
  // http://superuser.com/questions/443406/how-can-i-produce-high-cpu-load-on-a-linux-server
  //
  //
  // https://software.intel.com/sites/products/documentation/doclib/tbb_sa/help/reference/task_scheduler/task_scheduler_init_cls.htm#task_scheduler_init_cls
  task_scheduler_init init(task_scheduler_init::automatic); 
  // Trouble: "видимо у вас проблема в том, что вы ставите 486-е ядро, а оно видит только одно ядро процессора. ставьте 686-е"
  //   uname -r  # посмотреть что за ядро
  //
  {
    parallel_for(
      blocked_range<int>(0,records.size()), 
      ApplyFoo(&records[0], &raw_nodes[0]),
      simple_partitioner());
  }
  return raw_nodes;
}

vector<pair<int, Neighbors> > process_serial(const vector<string>& records) {
  vector<pair<int, Neighbors> > tmp(records.size());
  if (true) {
    //for (int i = 0; i < 2000; ++i)
    { 
      transform(records.begin(), records.end(),
	    tmp.begin(),
	    RawYieldFunctor());
    } 
  }
  return tmp;
}
}  // namespace ..persistency

class NodeInfo {
public:
  NodeInfo() : d(numeric_limits<int>::max()), visited(false) { } 
  int d;
  bool visited;
};

std::ostream& operator<<(std::ostream& os, const NodeInfo& obj)
{
  os << "(" << obj.d << ", " << obj.visited << ")";//<< endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const vector<NodeInfo>& obj) {
  for_each(begin(obj), end(obj), [&os] (const NodeInfo& info) {
    os << info;
  });
  os << endl;
  return os;
};


vector<graph_persistency::Neighbors> build_graph(const vector<string>& records) {
  // Не обязательно сортированные, поэтому граф строится отдельно
  using graph_persistency::Neighbors;
  vector<pair<int, Neighbors> > raw_nodes = graph_persistency::process_parallel(records);
  assert(!raw_nodes.empty());

  // CHECK_POINT
  // Все номера исходящих узвлов уникальны
  set<int> unique_check;
  auto action = [&unique_check] (const pair<int, Neighbors>& val) { 
    unique_check.insert(val.first); 
  };

  for_each(begin(raw_nodes), end(raw_nodes), action);
  assert(unique_check.size() == raw_nodes.size());

  // Формирование графа, если узлы уникальны, то можно параллельно записать в рабочий граф.
  // Нужен нулевой индекс.
  vector<Neighbors> graph(raw_nodes.size()+1);  // 0 добавляем как фейковый узел
  typedef std::pair<int, Neighbors> value_type;
  foreach(const value_type elem, raw_nodes) {
    graph[elem.first] = elem.second;
  }
  return graph;  
}


int main() 
{
  using graph_persistency::Neighbors;
  using graph_persistency::extract_records;
  
  try {
    /// IO and build graph
    // DbC debug only!
    vector<string> records = extract_records("../input_data/dijkstraData_test.txt");
    vector<Neighbors> graph = build_graph(records);
   
    // Validate graph
    assert(true);

    vector<NodeInfo> track(graph.size());
    int v_current = 1;
    track[v_current].d = 0;
    for (int i = 1, gr_size = graph.size(); i < gr_size; ++i) {  // цикл не такой
      track[v_current].visited = true;      
      Neighbors neighbors = graph[v_current];
      int d_root = track[v_current].d;
      
      // Обходим соседей
      vector<pair<int, int> > v;  // Number, dist
      foreach(Neighbor u, neighbors) {
	int w = u.end;
	if (!track[w].visited) {
	  int d_current = track[w].d;  // текущее расстояние
	  int d_new = d_root + u.weight;
	  if (d_current > d_new)
	    track[w].d = d_new; 
	  
	  v.push_back(make_pair(w, track[w].d));
	}
      }
      
      // Поиск минимальной
      pair<int, int> min_val = *min_element(begin(v), end(v), 
	[](const pair<int, int>& o1, const pair<int, int>& o2) -> bool {
	  return o1.second < o2.second;
	}
      );
      
      v_current = min_val.first;
      cout << track;
    }
  } catch (const exception& e) {
    cout << e.what() << endl;
  }  
  return 0;
}


/// With heap
// https://sites.google.com/site/indy256/algo_cpp/dijkstra_heap
// http://stackoverflow.com/questions/23592252/implementing-dijkstras-shortest-path-algorithm-using-c-and-stl


