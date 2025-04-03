#include <bits/stdc++.h>
using namespace std;

map<char, set<char>> first, follow;
map<char, vector<string>> productions;
set<char> nonTerminals, terminals;
char startSymbol;

void computeFirst(char symbol) {
    if (!isupper(symbol)) {
        first[symbol].insert(symbol);
        return;
    }

    for (auto prod : productions[symbol]) {
        for (int i = 0; i < prod.length(); i++) {
            char ch = prod[i];
            if (ch == 'e') {  // Using 'e' instead of 'ε'
                first[symbol].insert('e');
                break;
            } else if (!isupper(ch)) {
                first[symbol].insert(ch);
                break;
            } else {
                computeFirst(ch);
                bool hasEpsilon = false;
                for (char f : first[ch]) {
                    if (f != 'e') first[symbol].insert(f);
                    else hasEpsilon = true;
                }
                if (!hasEpsilon) break;
            }
        }
    }
}

void computeFollow(char symbol) {
    if (symbol == startSymbol) follow[symbol].insert('$');

    for (auto prod : productions) {
        char lhs = prod.first;
        for (auto rhs : prod.second) {
            for (int i = 0; i < rhs.length(); i++) {
                if (rhs[i] == symbol) {
                    bool hasEpsilon = true;
                    for (int j = i + 1; j < rhs.length(); j++) {
                        char next = rhs[j];
                        for (char f : first[next]) {
                            if (f != 'e') follow[symbol].insert(f);
                        }
                        if (first[next].count('e') == 0) {
                            hasEpsilon = false;
                            break;
                        }
                    }
                    if (hasEpsilon && lhs != symbol) {
                        computeFollow(lhs);
                        for (char f : follow[lhs]) {
                            follow[symbol].insert(f);
                        }
                    }
                }
            }
        }
    }
}

void constructParsingTable(map<pair<char, char>, string> &table) {
    for (auto prod : productions) {
        char lhs = prod.first;
        for (auto rhs : prod.second) {
            set<char> firstSet;
            bool hasEpsilon = true;

            for (char ch : rhs) {
                if (!isupper(ch)) {
                    firstSet.insert(ch);
                    hasEpsilon = false;
                    break;
                } else {
                    for (char f : first[ch]) {
                        if (f != 'e') firstSet.insert(f);
                    }
                    if (first[ch].count('e') == 0) {
                        hasEpsilon = false;
                        break;
                    }
                }
            }

            for (char t : firstSet) {
                table[{lhs, t}] = rhs;
            }

            if (hasEpsilon || rhs == "e") {
                for (char f : follow[lhs]) {
                    table[{lhs, f}] = "e";
                }
            }
        }
    }
}

int main() {
    int n;
    cout << "Enter number of productions: ";
    cin >> n;

    cout << "Enter the productions (e.g., E->TX):\n";
    for (int i = 0; i < n; i++) {
        string prod;
        cin >> prod;
        char lhs = prod[0];
        string rhs = prod.substr(3);
        stringstream ss(rhs);
        string token;
        while (getline(ss, token, '|')) {
            productions[lhs].push_back(token);
        }
        nonTerminals.insert(lhs);
    }

    cout << "Enter start symbol: ";
    cin >> startSymbol;

    for (char nt : nonTerminals) computeFirst(nt);
    for (char nt : nonTerminals) computeFollow(nt);

    cout << "\nFIRST Sets:\n";
    for (auto it : first) {
        cout << "FIRST(" << it.first << ") = { ";
        for (char ch : it.second) cout << (ch == 'e' ? "ε" : string(1, ch)) << " ";
        cout << "}\n";
    }

    cout << "\nFOLLOW Sets:\n";
    for (auto it : follow) {
        cout << "FOLLOW(" << it.first << ") = { ";
        for (char ch : it.second) cout << ch << " ";
        cout << "}\n";
    }

    map<pair<char, char>, string> parsingTable;
    constructParsingTable(parsingTable);

    cout << "\nPredictive Parsing Table:\n";
    for (auto it : parsingTable) {
        cout << "M[" << it.first.first << ", " << it.first.second << "] = "
             << (it.second == "e" ? "ε" : it.second) << endl;
    }

    return 0;
}
