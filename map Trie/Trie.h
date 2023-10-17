#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

using namespace std;

class Trie{
public:
  virtual void insert(const string &)=0;
  virtual bool search(const string &)=0;
  virtual bool remove(const string &)=0;
  virtual vector<string> getAll()=0;
  virtual vector<string> getKTopMatches(const string &, int)=0;

};

#endif
