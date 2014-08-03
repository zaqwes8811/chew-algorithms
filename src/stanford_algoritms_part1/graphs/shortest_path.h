
#ifndef P1_GR_SHORTEST_PATH
#define P1_GR_SHORTEST_PATH

// Named params in ctor:
// http://marcoarena.wordpress.com/2011/05/27/learn-how-to-support-named-parameters-in-cpp/
class EdgeMaker;  // TODO: bad but... don't work anyway
struct Edge {
  Edge() : weight(), end() {}
  int weight;
  int end;  
  bool operator==(const Edge& that) const { return (that.weight == weight) && (that.end == end); }
  Edge(const EdgeMaker& maker);  // реализацию вынести обязательно!
};


class EdgeMaker {
public:
  // выдает предупреждение, если инициализация не в таком порядке как объя. в классе
  EdgeMaker() : weight_(0), end_(0)  { }  
  
  EdgeMaker& end(int end) { end_ = end; return *this;}
  EdgeMaker& weight(int weight) { weight_ = weight; return *this; }
  Edge create();  // лучше, т.к. не нужно лезть в класс, который создается, но запись длиннее
  
private:
  friend class Edge;
  int weight_;
  int end_;
};

// Overhead
/*
// Обработчик одного узла для for_each
class OneNodeFunctor {
  //int index;
public:
  OneNodeFunctor();
  
};*/


// Failed
/*
template <typename T>
void print(const T& val) {
  cout << val << endl;
}

//template <typename T>
void print_vec(const vector<int>& val) {
  for_each(val.begin(), val.end(), print);  
}*/

#endif