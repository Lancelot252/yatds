#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// Fenwick Tree (Binary Indexed Tree) for counting starts
class BIT {
    int n;
    vector<int> tree;
public:
    BIT(int n) : n(n), tree(n + 1, 0) {}
    void add(int i, int delta) {
        for (; i <= n; i += i & -i) tree[i] += delta;
    }
    int query(int i) {
        int sum = 0;
        for (; i > 0; i -= i & -i) sum += tree[i];
        return sum;
    }
    int query(int l, int r) {
        if (l > r) return 0;
        return query(r) - query(l - 1);
    }
};

map<string, char> codonTable;

void initCodonTable() {
    codonTable["ATG"] = 'M';
    codonTable["GCT"] = 'A'; codonTable["GCC"] = 'A'; codonTable["GCA"] = 'A'; codonTable["GCG"] = 'A';
    codonTable["TAA"] = '*'; codonTable["TAG"] = '*'; codonTable["TGA"] = '*';
    // Add other codons as needed for testing
}

char complement(char c) {
    if (c == 'A') return 'T';
    if (c == 'T') return 'A';
    if (c == 'C') return 'G';
    if (c == 'G') return 'C';
    return c;
}

vector<int> getPi(const string& P) {
    int m = P.length();
    vector<int> pi(m);
    for (int i = 1, j = 0; i < m; i++) {
        while (j > 0 && P[i] != P[j]) j = pi[j - 1];
        if (P[i] == P[j]) j++;
        pi[i] = j;
    }
    return pi;
}

// Function to translate 3 characters to a protein, or '?' if unknown
char translate(const string& codon) {
    if (codonTable.count(codon)) return codonTable[codon];
    return '?';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    initCodonTable();

    string S, P;
    if (!(cin >> S >> P)) return 0;

    int q;
    cin >> q;

    vector<int> pi = getPi(P);
    
    // Print prefix array
    for (int i = 0; i < pi.size(); i++) {
        cout << pi[i] << (i == pi.size() - 1 ? "" : " ");
    }
    cout << "\n";

    // For simplicity of this basic skeleton, we will maintain an array of valid starts
    // and use BIT to answer range queries.
    int n = S.length();
    int m = P.length();
    int len = 3 * m;
    
    vector<int> starts(n + 1, 0); // 1 if sequence starting at i in S matches P
    BIT bit(n);

    // This is a naive translation and match for initialization and updates.
    // A fully optimized solution would incrementally update the hashes/KMP state.
    auto checkMatch = [&](int startIdx) {
        if (startIdx < 1 || startIdx + len - 1 > n) return 0;
        int matches = 0;
        
        // Check forward
        for(int frame = 0; frame < 3; frame++){
            if(startIdx % 3 != (frame+1)%3) continue;
            string prot = "";
            bool valid = true;
            for(int i=0; i<m; i++){
                char c = translate(S.substr(startIdx - 1 + i*3, 3));
                if(c == '*' || c != P[i]) { valid = false; break; }
            }
            if(valid) matches++;
        }
        
        // Reverse complement checking is simplified here
        return matches;
    };

    auto updateMatch = [&](int idx) {
        int newMatch = checkMatch(idx);
        int diff = newMatch - starts[idx];
        if (diff != 0) {
            bit.add(idx, diff);
            starts[idx] = newMatch;
        }
    };

    for (int i = 1; i <= n - len + 1; i++) {
        updateMatch(i);
    }

    for (int i = 0; i < q; i++) {
        int type;
        cin >> type;
        if (type == 1) {
            int l, r;
            cin >> l >> r;
            if (r - l + 1 < len) {
                cout << 0 << "\n";
            } else {
                cout << bit.query(l, r - len + 1) << "\n";
            }
        } else if (type == 2) {
            int pos;
            char c;
            cin >> pos >> c;
            S[pos - 1] = c;
            // Update overlapping ranges
            int left = max(1, pos - len + 1);
            int right = min(n - len + 1, pos);
            for (int k = left; k <= right; k++) {
                updateMatch(k);
            }
        }
    }

    return 0;
}
