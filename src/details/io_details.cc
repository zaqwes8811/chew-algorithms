#include "details/io_details.h"

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
// istream::sentry example
#include <iostream>     // std::istream, std::cout
#include <string>       // std::string
#include <sstream>      // std::stringstream
#include <locale>       // std::isspace, std::isdigit
#include <vector>
#include <algorithm>

// App

namespace try_deserialize {
  using namespace std;
struct Phone {
  std::string digits;
};

// custom extractor for objects of type Phone
std::istream& operator>>(std::istream& is, Phone& tel)
{
    std::istream::sentry s(is);
    if (s) while (is.good()) {
      char c = is.get();
      if (std::isspace(c,is.getloc())) break;
      if (std::isdigit(c,is.getloc())) tel.digits += c;
    }
    return is;
}

int test() {
  string w1_1_filename = "../stanford_algoritms_part2/in_data/jobs.txt";
  fstream stream(w1_1_filename.c_str());
  if (!stream)
    throw runtime_error("Error: can't open file");
  
  string line;
  while (!stream.fail()) {
    getline(stream, line);  // разделитель что угодно
    
    //if (stream.peek() == ' ')
     // stream.ignore();
    
    cout << line << endl;
  }
  
  /// Extract objects
  // http://www.cplusplus.com/reference/istream/istream/sentry/
  std::stringstream parseme ("   (555)2326");
  Phone myphone;
  parseme >> myphone;
  std::cout << "digits parsed: " << myphone.digits << '\n';
  return 0;
}

}  // namespace

namespace io_details {
  using namespace std;
  
/*
TEST: 
  string test_line("0\t8,89\t9,90\t\n");
  stringstream ss;
  pair<int, Neighbors> test = parse_node_data(test_line, ss);
  assert(test.second.size() == 2);
 */

// TODO: Возвращаемое значение не всегда копируется?
pair<int, vector<Arrow> > parse_node_data(const string& line, stringstream& ss) 
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

  vector<Arrow> result;  // TODO: to deque
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
    
    result.push_back(Arrow(i, j));//EdgeMaker().end(i).weight(j));
  }

  //make_pair(max_node_idx
  return make_pair(root, result); // сразу не поставил а gcc не отловил - в итоге дамп памяти
}
  
}  // namespace

