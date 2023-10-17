#include "mapTrie.h"
#include <iostream>
#include <algorithm>

using namespace std;

mapTrie::mapTrie(){
}

mapTrie::~mapTrie(){
}

void mapTrie::insert(const string & s){
  node* aux = root;
  if (aux == NULL) return;

  for(int i = 0; i < s.size(); i++){
    if(aux->mapa.find(s[i]) == aux->mapa.end()){
      aux->mapa[s[i]] = new node();
    }
    aux = aux->mapa[s[i]];
  }
  aux->endCount++;
}

bool mapTrie::search(const string & s){
  node* aux = root;
  if (aux == NULL) return false;
  for(int i = 0; i < s.size(); i++){
    if(aux->mapa.find(s[i]) == aux->mapa.end()){
      return false;
    }
    aux = aux->mapa[s[i]];
  }

  if(aux->endCount > 0) return true;
  return false;
}

bool mapTrie::removeAux(node* aux, const string& s, int level) {
   if(!aux) return false;
   if(s.size() == level) {
       if(aux->endCount == 0) return false;
         aux->endCount = 0;
         return aux->mapa.empty();
       }

  bool mustDelete = removeAux(aux->mapa[s[level]], s, level + 1);

    if (mustDelete) {
      auto it = aux->mapa.find(s[level]);
      aux->mapa.erase(it);
      return aux->mapa.empty() && !(aux->endCount > 0);
    }
    return false;
}

bool mapTrie::remove(const string & s){
  node* aux = root;
  if(!aux) return false;
  if(search(s)){
    removeAux(aux, s, 0);
    return true;
  }
  return false;

}

void mapTrie::getAllAux(node* aux, string& auxStr, vector<string>& v){
  if(aux->endCount > 0) v.push_back(auxStr);

  for(map<char,node*>::iterator it = aux->mapa.begin(); it != aux->mapa.end(); it++){
    auxStr.push_back(it->first);
    getAllAux(it->second, auxStr, v);
    auxStr.pop_back();
  }
}

vector<string> mapTrie::getAll(){
  vector<string> v;
  string s;
  getAllAux(root, s, v);

  return v;
}

void mapTrie::getKTopMatchesAux(node* aux, string& auxStr, map<string,int>& mp){
  if(aux->endCount > 0) mp[auxStr] = aux->endCount;

  for(map<char,node*>::iterator it = aux->mapa.begin(); it != aux->mapa.end(); it++){
    auxStr.push_back(it->first);
    getKTopMatchesAux(it->second, auxStr, mp);
    auxStr.pop_back();
  }
}

vector<string> mapTrie::getKTopMatches(const string & s, int k){
  node* aux = root;
  map<string,int> mp;
  vector<string> vTopMatches;
  vector<string> emptyVector;
  string auxStr;

  for(int i = 0; i < s.size(); i++){
    if(aux->mapa.find(s[i]) != aux->mapa.end()){
      aux = aux->mapa[s[i]];
      auxStr.push_back(s[i]);
    }
    else{
      return emptyVector;
    }
  }
  getKTopMatchesAux(aux, auxStr, mp);

  for(int i = 0; i < k; i++){
    auto max = max_element(mp.begin(), mp.end(), [](const auto &x, const auto &y){
                return x.second < y.second;
    });

    if(max == mp.end()) break;
    vTopMatches.push_back(max->first);
    mp.erase(max);
  }

  return vTopMatches;
}
