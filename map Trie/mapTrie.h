#ifndef MAP_TRIE_H
#define MAP_TRIE_H

#include "Trie.h"
#include <map>
#include <iostream>

using namespace std;

class node{
public:
  int endCount;
  map<char, node*> mapa;
  node(){
    endCount = 0;
  }
  ~node(){
  }
};

class mapTrie : public Trie{

public:
  mapTrie();
  ~mapTrie();
  void insert(const string & s);
  bool search(const string & s);
  bool remove(const string & s);
  vector<string> getAll();
  vector<string> getKTopMatches(const string & s, int k);
  bool removeAux(node* aux, const string & s, int level);
  void getAllAux(node* aux, string& auxStr, vector<string>& v);
  void getKTopMatchesAux(node* aux, string& auxStr, map<string,int>& mp);

private:
  node* root = new node;

};

#endif
