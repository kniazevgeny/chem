#include "ParseMolecule.h"
#include <regex>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <queue>


const std::map<std::string, std::vector<int>> table{
        {"H",  {1,  -1}},
        {"He", {1}},
        {"Li", {1}},
        {"Be", {2}},
        {"B",  {3}},
        {"C",  {-4, 2,  4}},
        {"N",  {-3, 1,  2, 3, 4, 5}},
        {"O",  {-2, -1, 2}},
        {"F",  {-1}},
        {"Ne", {0}},
        {"Na", {1}},
        {"Mg", {2}},
        {"Al", {3}},
        {"Si", {-4, 2,  4}},
        {"P",  {-3, 3,  5}},
        {"S",  {-2, 1,  2, 4, 5, 6}},
        {"Cl", {-1, 1,  3, 4, 5, 6, 7}},
        {"Ar", {0}},
        {"K",  {1}},
        {"Ca", {2}},
        {"Sc", {3}},
        {"Ti", {2,  3,  4}},
        {"V",  {2,  3,  4, 5}},
        {"Cr", {2,  3,  4, 6}},
        {"Mn", {2,  3,  4, 6, 7}},
        {"Fe", {2,  3,  6}},
        {"Co", {2,  3}},
        {"Ni", {2,  3}},
        {"Cu", {1,  2}},
        {"Zn", {2}}};
const std::map<std::string, int> ions{
        {"OH",     -1},
        {"HS",     -1},
        {"SO3",    -2},
        {"HSO3",   -1},
        {"SO4",    -2},
        {"HSO4",   -1},
        {"NO3",    -1},
        {"NO2",    -1},
        {"PO4",    -3},
        {"HPO4",   -2},
        {"H2PO4",  -1},
        {"CO3",    -2},
        {"HCO3",   -1},
        {"CH3COO", -1},
        {"SiO3",   -2},
        {"MnO4",   -1},
        {"Cr2O7",  -2},
        {"CrO4",   -2},
        {"ClO",    1},
        {"ClO2",   1},
        {"ClO3",   1},
        {"ClO4",   1},
        {"NH4",    1},
};

struct oxyStruct {
    int oxy;
    int count;
};



//std::vector<std::string> GetMapKeys(auto m) {
//  std::vector<std::string> keys;
//  for(auto const& i: m)
//    keys.emplace_back(i.first);
//  return keys;
//}

template<typename TV>
std::vector<std::string> GetMapKeys(std::map<std::string, TV> const &input_map) {
  std::vector<std::string> keys;
  for (auto const &element : input_map) {
    keys.emplace_back(element.first);
  }
  return keys;
}


bool isElementInVector(const std::vector<std::string> &vector, const std::string &element) {
  for (auto i: vector)
    if (element == i) return true;
  return false;
}


std::vector<std::string>
Splitter(std::string s, std::string re_str = "([A-Z][a-z]\\d+)|([A-Z][a-z])|([A-Z]\\d+)|([A-Z])") {
  std::smatch m;
  const std::regex re(re_str);
  std::vector<std::string> v;
  while (std::regex_search(s, m, re)) {
    v.emplace_back(m[0]);
    s = m.suffix().str();
  }
  return v;
}


std::pair<std::string, std::vector<int>>
GetElementValues(const std::string &word) {

  std::string element = Splitter(word, "\\D+")[0];
  int multiplier = 1;
  if (Splitter(word, "\\d+").size())
    multiplier = std::stoi(Splitter(word, "\\d+")[0]);
  std::vector<int> multipliedTable = table.at(element);
  for (size_t i = 0; i < multipliedTable.size(); i++)
    multipliedTable[i] *= multiplier;
  return std::make_pair(element, multipliedTable);
}


std::pair<std::map<std::string, std::vector<int>>, std::map<std::string, oxyStruct>>
GetOxidation(std::vector<std::pair<std::string, std::vector<int>>> parts, int shouldBe, bool returnReal) {
  /*
  Get real oxidation for each part

  Parameters:
  parts ([('Ca', [2])])
  should_be (int): Right part of equation. OH- should be -1
  return_real (bool): whether return values to construct molecule or calc oxidation

  Exit codes:
  2: impossible to calculate oxidation

   If returnReal == true, check pair.second. Otherwise, pair.first
   */
  std::vector<int> indices(parts.size());
  std::vector<int> indicesLimit(parts.size());
  int oxidationCounter = 0;
  for (size_t i = 0; i < parts.size(); i++) {
    oxidationCounter += parts[i].second[indices[i]];
    indicesLimit[i] = parts[i].second.size();
  }
  try {
    while (oxidationCounter != shouldBe) {
      int i = 0;
      while (true) {
        if (i == indices.size()) throw std::out_of_range("Kaboom");
        indices[i] += 1;
        if (indices[i] == indicesLimit[i]) {
          indices[i] = 0;
          i += 1;
        } else break;
      }
      oxidationCounter = 0;
      for (size_t part_i = 0; part_i < parts.size(); part_i++) {
        oxidationCounter += parts[part_i].second[indices[part_i]];
        if (oxidationCounter >= 9999) break;
      }
    }
  }
  catch (std::out_of_range) {
    std::cout << "\nImpossible to GetOxidation";
  }
  std::map<std::string, oxyStruct> theRealOxidation;
  std::map<std::string, std::vector<int>> oxidation;
  if (returnReal) {
    // Merge similar
    while (true) {
      bool hasBroken = false;
      for (size_t i = 0; i < parts.size(); i++) {
        for (size_t j = 0; j < parts.size(); j++) {
          if (i != j && parts[i].first == parts[j].first && parts[i].second[indices[i]] == parts[j].second[indices[j]]) {
            parts[i].second[indices[i]] += parts[j].second[indices[j]];
            parts.erase(parts.begin() + j);
            indices.erase(indices.begin() + j);
            indicesLimit.erase(indicesLimit.begin() + j);
            hasBroken = true;
            // just break 2 loops
            break;
          }
        }
        if (hasBroken) break;
      }
      if (!hasBroken) break;
    };
    for (size_t part_i = 0; part_i < parts.size(); part_i++) {
      oxyStruct st;
      st.oxy = table.at(parts[part_i].first)[indices[part_i]];
      st.count = parts[part_i].second[indices[part_i]] / table.at(parts[part_i].first)[indices[part_i]];
      theRealOxidation[parts[part_i].first] = st;
    }
    return std::make_pair(oxidation, theRealOxidation);
  }
  for (size_t part_i = 0; part_i < parts.size(); part_i++) {
    std::vector<int> oxy;
    for (int limit = 0; limit < indicesLimit[part_i]; limit++) {
      if (limit == indices[part_i])
        oxy.emplace_back(parts[part_i].second[indices[part_i]]);
      else oxy.emplace_back(9999);
    }
    oxidation[parts[part_i].first] = oxy;
  }
  return std::make_pair(oxidation, theRealOxidation);
}


std::vector<std::pair<std::string, std::vector<int>>> CountValent(std::string inp) {
  /*
  Sapmles: Ca(NO3)2, H2SO4, Cu(OH)2CO3
  Returns: (for MgSO4) {'S': {'oxy': 6, 'count': 1}, 'O': {'oxy': -2, 'count': 4},...}
  */
  std::vector<std::string> seqs = Splitter(inp, "([(].+[)]\\d+)|([^()]+)");
  std::vector<std::pair<std::string, std::vector<int>>> parts;
  std::vector<std::string> foundIons;
  for (std::string seq : seqs) {
    std::cout << " " << seq;
    if (seq.find('(') != std::string::npos) {
      int bracketMultiplier = (int) (Splitter(seqs[1], "[)]\\d+")[0][1] - '0');
      std::string seqNoBrackets = seq.substr(seq.find('(') + 1, seq.find(')'));
      // Это точно работает? ^
      std::vector<std::pair<std::string, std::vector<int>>> bracketDigits;
      if (isElementInVector(GetMapKeys(ions), seqNoBrackets)) {
        foundIons.emplace_back(seqNoBrackets);
        // if OH -, CO3 -2 in ions
        // removes ('O', [-2, -1, 2]) -1 and 2 to simplify calculations
        std::vector<std::pair<std::string, std::vector<int>>> tmpParts;
        for (std::string i: Splitter(seqNoBrackets))
          tmpParts.emplace_back(GetElementValues(i));
        auto tmpOxidations = GetOxidation(tmpParts, ions.at(seqNoBrackets), false).first;
        // cout...
        for (std::string i: GetMapKeys(tmpOxidations))
          bracketDigits.emplace_back(std::make_pair(i, tmpOxidations[i]));
      } else
        for (std::string i: Splitter(seqNoBrackets))
          bracketDigits.emplace_back(GetElementValues(i));
      for (const auto &bd: bracketDigits) {
        std::vector<int> v;
        for (auto i: bd.second)
          v.emplace_back(i * bracketMultiplier);
        parts.emplace_back(std::make_pair(bd.first, v));
      }
    } else {
      // Look for standard ions in the sequence
      for (std::string ion : GetMapKeys(ions)) {
        while (seq.find(ion) != std::string::npos) {
          foundIons.emplace_back(ion);
          seq.erase(seq.find(ion), ion.size());
          std::vector<std::pair<std::string, std::vector<int>>> tmpParts;
          for (auto i : Splitter(ion)) {
            tmpParts.emplace_back(GetElementValues(i));
          }
          std::map<std::string, std::vector<int>> tmpOxidations = GetOxidation(tmpParts, ions.at(ion), false).first;
          for (auto i: GetMapKeys(tmpOxidations))
            parts.emplace_back(i, tmpOxidations[i]);
        }
      }
      for (const auto &i: Splitter(seq))
        parts.emplace_back(GetElementValues(i));
    }
  }
  // cout ions
  return parts;
}

bool CheckIsConnective(std::vector<std::vector<int>> g) {
  std::queue<int> q;
  q.push(0);
  std::vector<bool> visited(g.size(), false);
  while (!q.empty()) {
    int v = q.front();
    q.pop();
    for (int i : g[v])
      if (!visited[i]) {
        visited[i] = true;
        q.push(i);
      }
  }
  for (bool b : visited)
    if (!b) return false;
  return true;
}

bool CheckCharge(std::vector<int> *charge, std::vector<std::string> elementById, int x, int y) {
  // Returns True if connection by charge is possible
  // Organic molecules
  if (elementById[x] == "P" || elementById[y] == "P")
    return true;
  if (elementById[x] == "C" || elementById[y] == "C")
    return true;
  // Nitrogen?
  // Peroxide
  if (elementById[x] == "O" && charge->at(x) == -1 || elementById[y] == "O" && charge->at(y) == -1)
    return true;
  // For everything else
  if (charge->at(x) * charge->at(y) < 0)
    return true;
  return false;
}

std::pair<bool, std::vector<std::vector<int>>>
BuildGraphRec(std::vector<std::vector<int>> connections, const std::vector<std::string> &elementById,
              std::vector<int> *charge) {
  // Main idea: for every element two ways: take or not

  // Check if connections are good
  bool areGood = true;
  for (const auto &c: connections)
    for (int c1: c)
      if (c1 == -1) {
        areGood = false;
        break;
      }
  if (areGood) {
    std::vector<std::vector<int>> t;
    if (!CheckIsConnective(connections)) return std::make_pair(false, t);
    return std::make_pair(true, connections);
  }
  for (size_t v = 0; v < connections.size(); v++)
    for (size_t i = 0; i < connections[v].size(); i++)
      if (connections[v][i] == -1)
        for (size_t j = 0; j < elementById.size(); j++) {
          bool isMinusOneInConnections = false;
          for (int t: connections[j])
            if (t == -1) isMinusOneInConnections = true;
          if (v != j && isMinusOneInConnections && CheckCharge(charge, elementById, j, v)) {
            connections[v][i] = j;
            size_t u;
            for (u = 0; u < connections[j].size(); u++)
              if (connections[j][u] == -1) {
                connections[j][u] = v;
                break;
              }
            auto result = BuildGraphRec(connections, elementById, charge);
            if (result.first)
              return result;
            connections[v][i] = -1;
            connections[j][u] = -1;
          }
        }
  return std::make_pair(false, connections);
}

std::pair<std::vector<std::vector<int>>, std::vector<std::string>>
BuildGraph(const std::map<std::string, oxyStruct> &oxidations) {
  /*
  Builds a graph based on oxidations array.
  Graph can be used to visualize a molecule in 2d or 3d space

  Parameters:
  oxidations: {'O': {'oxy': -2, count: 2}, ...}
   */

  // Map to Vector<pairs>
  std::vector<std::pair<std::string, oxyStruct>> oxidationsV;
  for (auto const &kv : oxidations)
    oxidationsV.emplace_back(std::make_pair(kv.first, kv.second));

  // Sort by oxy value
  std::sort(oxidationsV.begin(),
            oxidationsV.end(),
            [](std::pair<std::string, oxyStruct> const &a, std::pair<std::string, oxyStruct> const &b) {
                return abs(a.second.oxy) > abs(b.second.oxy);
            });

  std::vector<std::vector<int>> connections;
  std::vector<std::string> elementById;
  std::vector<int> charge;
  for (const auto& v: oxidationsV)
    for (int c = 0; c < v.second.count; c++) {
      std::vector<int> t(abs(v.second.oxy), -1);
      connections.emplace_back(t);
      elementById.emplace_back(v.first);
      charge.emplace_back(v.second.oxy);
    }
  return std::make_pair(BuildGraphRec(connections, elementById, &charge).second, elementById);
}

std::pair<std::vector<std::vector<int>>, std::vector<std::string>>
ParseMolecule(std::string inp) {
  std::vector<std::pair<std::string, std::vector<int>>> parts = CountValent(inp);
  auto valents = GetOxidation(parts, 0, true).second;
  return BuildGraph(valents);
  // for (size_t v = 0; v < graph.size(); v++) {
  //   std::cout << std::endl;
  //   std::cout << decodeInfo[v] << " ";
  //   for (auto g: graph[v])
  //     std::cout << g << " ";
  // } 
}