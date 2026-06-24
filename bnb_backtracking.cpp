#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// Struktur data barang
struct Item {
    int weight;
    int value;
};

// Struktur data node untuk Branch and Bound
struct Node {
    int level;
    int profit;
    int weight;
    double bound;
};

// Fungsi perbandingan untuk mengurutkan berdasarkan rasio value/weight tertinggi
bool cmp(Item a, Item b) {
    double r1 = (double)a.value / a.weight;
    double r2 = (double)b.value / b.weight;
    return r1 > r2;
}

// Komparator untuk Priority Queue (Max-Heap berdasarkan Bound terbesar)
struct CompareBound {
    bool operator()(Node const& a, Node const& b) {
        return a.bound < b.bound;
    }
};

// Variabel global untuk analisis perbandingan jumlah node yang dikunjungi
int node_backtracking_count = 0;
int node_bb_count = 0;

// --- FUNGSI UTAMA BRANCH AND BOUND ---
double calculateBound(Node u, int n, int W, const vector<Item>& arr) {
    if (u.weight >= W) return 0;

    double profit_bound = u.profit;
    int j = u.level + 1;
    int totweight = u.weight;

    while ((j < n) && (totweight + arr[j].weight <= W)) {
        totweight += arr[j].weight;
        profit_bound += arr[j].value;
        j++;
    }

    if (j < n) {
        profit_bound += (W - totweight) * ((double)arr[j].value / arr[j].weight);
    }

    return profit_bound;
}

int knapsackBranchAndBound(int W, const vector<Item>& original_arr, int n) {
    vector<Item> arr = original_arr;
    sort(arr.begin(), arr.end(), cmp); // Pengurutan penting untuk efisiensi BnB

    priority_queue<Node, vector<Node>, CompareBound> pq;
    Node u, v;

    u.level = -1;
    u.profit = 0;
    u.weight = 0;
    u.bound = calculateBound(u, n, W, arr);
    
    int maxProfit = 0;
    pq.push(u);
    node_bb_count++;

    while (!pq.empty()) {
        u = pq.top();
        pq.pop();

        if (u.level == n - 1) continue;
        if (u.bound <= maxProfit) continue; 

        // KASUS A: Barang berikutnya DIANGKUT
        v.level = u.level + 1;
        v.weight = u.weight + arr[v.level].weight;
        v.profit = u.profit + arr[v.level].value;

        if (v.weight <= W && v.profit > maxProfit) {
            maxProfit = v.profit;
        }

        v.bound = calculateBound(v, n, W, arr);
        if (v.bound > maxProfit) {
            pq.push(v);
            node_bb_count++;
        }

        // KASUS B: Barang berikutnya TIDAK DIANGKUT
        v.weight = u.weight;
        v.profit = u.profit;
        v.bound = calculateBound(v, n, W, arr);

        if (v.bound > maxProfit) {
            pq.push(v);
            node_bb_count++;
        }
    }
    return maxProfit;
}

// --- FUNGSI UTAMA BACKTRACKING (DFS dengan Pruning) ---
void backtrackingHelper(int level, int current_weight, int current_profit, int W, int n, const vector<Item>& arr, int& maxProfit) {
    node_backtracking_count++;

    // Base Case: jika sudah mengecek semua barang
    if (level == n) {
        if (current_profit > maxProfit) {
            maxProfit = current_profit;
        }
        return;
    }

    // Pangkas (Pruning): Jika profit saat ini ditambah estimasi sisa barang maksimal tidak bisa melewati maxProfit, stop cabang ini.
    // (Di sini menggunakan pruning sederhana berbasis bobot)
    
    // Pilihan 1: Angkut barang jika kapasitas cukup (Mengeksplorasi pohon ke kiri)
    if (current_weight + arr[level].weight <= W) {
        backtrackingHelper(level + 1, current_weight + arr[level].weight, current_profit + arr[level].value, W, n, arr, maxProfit);
    }

    // Pilihan 2: Tidak mengangkut barang (Mengeksplorasi pohon ke kanan)
    backtrackingHelper(level + 1, current_weight, current_profit, W, n, arr, maxProfit);
}

int knapsackBacktracking(int W, const vector<Item>& arr, int n) {
    int maxProfit = 0;
    node_backtracking_count = 0;
    backtrackingHelper(0, 0, 0, W, n, arr, maxProfit);
    return maxProfit;
}

int main() {
    int n, W;

    cout << "=====================================================" << endl;
    cout << "   PROGRAM PERBANDINGAN BACKTRACKING VS BRANCH & BOUND   " << endl;
    cout << "=====================================================" << endl;

    cout << "Masukkan jumlah barang: ";
    cin >> n;

    cout << "Masukkan kapasitas maksimal ransel (W): ";
    cin >> W;

    vector<Item> arr;
    cout << "\n--- Masukkan Data Barang ---" << endl;
    for (int i = 0; i < n; i++) {
        Item temp;
        cout << "Barang ke-" << i + 1 << " (Keuntungan/Profit): ";
        cin >> temp.value;
        cout << "Barang ke-" << i + 1 << " (Bobot/Weight): ";
        cin >> temp.weight;
        arr.push_back(temp);
        cout << "----------------------------" << endl;
    }

    // Eksekusi Kedua Algoritma
    int profitBT = knapsackBacktracking(W, arr, n);
    int profitBB = knapsackBranchAndBound(W, arr, n);

    cout << "\n====================== HASIL OUTPUT ======================" << endl;
    cout << "1. ALGORITMA BACKTRACKING:" << endl;
    cout << "   - Total Keuntungan Maksimal  : " << profitBT << endl;
    cout << "   - Jumlah Node yang Dikunjungi: " << node_backtracking_count << " node" << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "2. ALGORITMA BRANCH AND BOUND:" << endl;
    cout << "   - Total Keuntungan Maksimal  : " << profitBB << endl;
    cout << "   - Jumlah Node yang Dikunjungi: " << node_bb_count << " node" << endl;
    cout << "==========================================================" << endl;

    return 0;
}