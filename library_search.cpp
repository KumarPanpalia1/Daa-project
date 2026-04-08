/*
 * ============================================================
 *   CASE STUDY 1: Smart Library Search Optimization
 *   Linear Search vs Binary Search
 *   Dataset: 50,000 Books (Unique Accession Numbers)
 * ============================================================
 *
 *  Covers:
 *    - Linear Search  (unsorted array, sequential scan)
 *    - Binary Search  (sorted array, halving strategy)
 *    - Best / Average / Worst case scenarios
 *    - Execution time, comparison count, sorting overhead
 *
 *  Compile:  g++ -O0 -o library_search library_search.cpp
 *  Run:      ./library_search
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;
using namespace std::chrono;

// ─────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────
const int DATASET_SIZE    = 50000;
const int ACCESSION_MIN   = 100000;   // 6-digit accession numbers
const int ACCESSION_MAX   = 999999;

// ─────────────────────────────────────────────
//  Search Result Structure
// ─────────────────────────────────────────────
struct SearchResult {
    int    position;          // index found (-1 if not found)
    long long comparisons;    // number of comparisons made
    double timeUs;            // time in microseconds
    bool   found;
};

// ─────────────────────────────────────────────
//  Dataset Generator
//  Produces DATASET_SIZE unique random accession numbers
// ─────────────────────────────────────────────
vector<int> generateDataset(int size) {
    mt19937 rng(42);  // fixed seed for reproducibility
    uniform_int_distribution<int> dist(ACCESSION_MIN, ACCESSION_MAX);

    vector<int> dataset;
    dataset.reserve(size);

    // Use a simple Fisher-Yates inspired unique fill
    // Generate a larger pool, shuffle, pick first `size`
    vector<int> pool;
    pool.reserve(ACCESSION_MAX - ACCESSION_MIN + 1);
    for (int i = ACCESSION_MIN; i <= ACCESSION_MAX; i++)
        pool.push_back(i);

    shuffle(pool.begin(), pool.end(), rng);
    dataset.assign(pool.begin(), pool.begin() + size);

    return dataset;   // intentionally UNSORTED
}

// ─────────────────────────────────────────────
//  Linear Search  —  O(n)
//  Works on unsorted OR sorted arrays
// ─────────────────────────────────────────────
SearchResult linearSearch(const vector<int>& arr, int key) {
    SearchResult res = {-1, 0, 0.0, false};

    auto start = high_resolution_clock::now();

    for (int i = 0; i < (int)arr.size(); i++) {
        res.comparisons++;
        if (arr[i] == key) {
            res.position = i;
            res.found    = true;
            break;
        }
    }

    auto end  = high_resolution_clock::now();
    res.timeUs = duration_cast<nanoseconds>(end - start).count() / 1000.0;

    return res;
}

// ─────────────────────────────────────────────
//  Binary Search  —  O(log n)
//  Requires a SORTED array
// ─────────────────────────────────────────────
SearchResult binarySearch(const vector<int>& arr, int key) {
    SearchResult res = {-1, 0, 0.0, false};

    auto start = high_resolution_clock::now();

    int low = 0, high = (int)arr.size() - 1;
    while (low <= high) {
        res.comparisons++;
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            res.position = mid;
            res.found    = true;
            break;
        } else if (arr[mid] < key) {
            low  = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    auto end  = high_resolution_clock::now();
    res.timeUs = duration_cast<nanoseconds>(end - start).count() / 1000.0;

    return res;
}

// ─────────────────────────────────────────────
//  Sorting with timing
// ─────────────────────────────────────────────
double sortDataset(vector<int>& arr) {
    auto start = high_resolution_clock::now();
    sort(arr.begin(), arr.end());
    auto end   = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / 1000.0;  // ms
}

// ─────────────────────────────────────────────
//  Display Helpers
// ─────────────────────────────────────────────
void printDivider(char c = '=', int width = 70) {
    cout << string(width, c) << "\n";
}

void printHeader(const string& title) {
    cout << "\n";
    printDivider();
    cout << "  " << title << "\n";
    printDivider();
}

void printResultRow(const string& label,
                    const SearchResult& ls,
                    const SearchResult& bs)
{
    cout << left  << setw(20) << label
         << right << setw(10) << ls.comparisons
         << setw(12) << fixed << setprecision(2) << ls.timeUs << " us"
         << setw(14) << bs.comparisons
         << setw(12) << fixed << setprecision(2) << bs.timeUs << " us"
         << "\n";
}

void printSearchDetail(const string& algo,
                       const string& cas,
                       int key,
                       const SearchResult& r)
{
    cout << "  Algorithm   : " << algo                              << "\n";
    cout << "  Case        : " << cas                               << "\n";
    cout << "  Search Key  : " << key                               << "\n";
    cout << "  Result      : " << (r.found ? "FOUND" : "NOT FOUND") << "\n";
    if (r.found)
        cout << "  Position    : index " << r.position              << "\n";
    cout << "  Comparisons : " << r.comparisons                    << "\n";
    cout << "  Time Taken  : " << fixed << setprecision(3)
                               << r.timeUs << " microseconds\n";
}

// ─────────────────────────────────────────────
//  Run All Cases for One Algorithm
// ─────────────────────────────────────────────
struct CaseSet {
    SearchResult best, average, worst;
};

CaseSet runLinearSearchCases(const vector<int>& arr) {
    CaseSet cs;
    // Best case  : first element of the array
    cs.best    = linearSearch(arr, arr.front());
    // Average    : element near the middle
    cs.average = linearSearch(arr, arr[arr.size() / 2]);
    // Worst case : element NOT in array (full scan)
    cs.worst   = linearSearch(arr, -1);   // -1 never exists
    return cs;
}

CaseSet runBinarySearchCases(const vector<int>& sortedArr) {
    CaseSet cs;
    // Best case  : element at exact midpoint (found in 1 comparison)
    int mid    = sortedArr[sortedArr.size() / 2];
    cs.best    = binarySearch(sortedArr, mid);

    // Average    : element at 3/4 position
    int avg    = sortedArr[sortedArr.size() * 3 / 4];
    cs.average = binarySearch(sortedArr, avg);

    // Worst case : element NOT in array (max comparisons = log2(n)+1)
    cs.worst   = binarySearch(sortedArr, -1);
    return cs;
}

// ─────────────────────────────────────────────
//  Step-by-Step Trace (first 20 steps)
// ─────────────────────────────────────────────
void traceLinearSearch(const vector<int>& arr, int key, int maxSteps = 20) {
    cout << "\n  [ Linear Search Trace — key: " << key << " ]\n";
    cout << "  " << string(48, '-') << "\n";
    cout << "  Step  |  Index  |  arr[i]  |  Match?\n";
    cout << "  " << string(48, '-') << "\n";

    int step = 0;
    for (int i = 0; i < (int)arr.size() && step < maxSteps; i++, step++) {
        bool match = (arr[i] == key);
        cout << "  " << setw(4) << (step + 1)
             << "  |  " << setw(5) << i
             << "  |  " << setw(6) << arr[i]
             << "  |  " << (match ? "<< FOUND >>" : "")
             << "\n";
        if (match) break;
    }
    if (step == maxSteps)
        cout << "  ... (trace limited to " << maxSteps << " steps)\n";
}

void traceBinarySearch(const vector<int>& arr, int key) {
    cout << "\n  [ Binary Search Trace — key: " << key << " ]\n";
    cout << "  " << string(64, '-') << "\n";
    cout << "  Step  |  Low   |  Mid   |  High  |  arr[mid]  |  Decision\n";
    cout << "  " << string(64, '-') << "\n";

    int low = 0, high = (int)arr.size() - 1, step = 0;
    while (low <= high) {
        step++;
        int mid = low + (high - low) / 2;
        string decision;
        if (arr[mid] == key) {
            decision = "FOUND";
        } else if (arr[mid] < key) {
            decision = "Go RIGHT";
            low = mid + 1;
        } else {
            decision = "Go LEFT";
            high = mid - 1;
        }
        cout << "  " << setw(4) << step
             << "  |  " << setw(5) << low
             << "  |  " << setw(5) << mid
             << "  |  " << setw(5) << high
             << "  |  " << setw(8) << arr[mid]
             << "  |  " << decision << "\n";
        if (decision == "FOUND") break;
    }
    if (low > high)
        cout << "  NOT FOUND — exhausted search space.\n";
}

// ─────────────────────────────────────────────
//  Final Analysis Summary
// ─────────────────────────────────────────────
void printAnalysis(CaseSet& ls, CaseSet& bs, double sortTimeMs) {

    printHeader("COMPARATIVE ANALYSIS");

    cout << "\n";
    cout << left  << setw(20) << "Case"
         << right << setw(10) << "LS Cmps"
         << setw(14) << "LS Time(us)"
         << setw(14) << "BS Cmps"
         << setw(14) << "BS Time(us)"
         << "\n";
    printDivider('-', 70);
    printResultRow("Best Case",    ls.best,    bs.best);
    printResultRow("Average Case", ls.average, bs.average);
    printResultRow("Worst Case",   ls.worst,   bs.worst);
    printDivider('-', 70);

    cout << "\n  Theoretical Complexity:\n";
    cout << "    Linear Search : Best O(1)  | Average O(n/2) | Worst O(n)\n";
    cout << "    Binary Search : Best O(1)  | Average O(log n) | Worst O(log n)\n";
    cout << "    Sort Overhead : O(n log n) — one-time cost\n";

    cout << "\n  Dataset Size      : " << DATASET_SIZE << " books\n";
    cout << "  Theoretical log2  : " << (int)ceil(log2(DATASET_SIZE)) << " (max binary search comparisons)\n";
    cout << "  Sort Time         : " << fixed << setprecision(3) << sortTimeMs << " ms\n";

    cout << "\n";
    printDivider('-', 70);
    cout << "  CONCLUSION\n";
    printDivider('-', 70);

    cout << R"(
  1. STATIC DATASETS (books rarely added/removed):
       Binary Search is clearly superior.
       Sort once (O(n log n)), then search in O(log n) every time.
       For 50,000 books, binary search uses ~16 comparisons vs ~25,000
       on average for linear search.

  2. DYNAMIC DATASETS (frequent insertions/deletions):
       Linear Search is more practical.
       Maintaining a sorted array for binary search is expensive
       (each insertion requires shifting elements — O(n) cost).
       Linear search works directly on an unsorted list.

  3. PREPROCESSING OVERHEAD:
       Sorting is a one-time cost that pays off after:
         Break-even ≈ sort_time / (linear_avg_time - binary_avg_time) queries
       For very few queries, linear search may be faster overall.

  4. RECOMMENDATION FOR UNIVERSITY LIBRARY:
       Since library accession numbers change infrequently,
       sort the database at system startup and use Binary Search
       for all student queries. This gives near-instant search
       results even with 50,000+ books.
)";
    printDivider();
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {

    // ── 1. Generate Dataset ──────────────────
    printHeader("STEP 1: Generating 50,000 Book Accession Numbers");
    cout << "  Generating unique random accession numbers...\n";
    vector<int> unsortedBooks = generateDataset(DATASET_SIZE);
    cout << "  Done! Sample (first 10): ";
    for (int i = 0; i < 10; i++)
        cout << unsortedBooks[i] << (i < 9 ? ", " : "\n");
    cout << "  Total books loaded: " << unsortedBooks.size() << "\n";


    // ── 2. LINEAR SEARCH (on unsorted array) ─
    printHeader("STEP 2: Linear Search on Unsorted Array");

    CaseSet ls = runLinearSearchCases(unsortedBooks);

    cout << "\n  --- Best Case (first element) ---\n";
    printSearchDetail("Linear Search", "Best", unsortedBooks.front(), ls.best);
    traceLinearSearch(unsortedBooks, unsortedBooks.front(), 5);

    cout << "\n  --- Average Case (middle element) ---\n";
    printSearchDetail("Linear Search", "Average", unsortedBooks[DATASET_SIZE/2], ls.average);

    cout << "\n  --- Worst Case (element NOT in array) ---\n";
    printSearchDetail("Linear Search", "Worst", -1, ls.worst);


    // ── 3. SORT THE ARRAY ────────────────────
    printHeader("STEP 3: Sorting the Dataset (Preprocessing Overhead)");
    vector<int> sortedBooks = unsortedBooks;   // copy
    double sortTimeMs = sortDataset(sortedBooks);
    cout << "  Sorting " << DATASET_SIZE << " books using std::sort (Introsort)...\n";
    cout << "  Sort completed in: " << fixed << setprecision(3) << sortTimeMs << " ms\n";
    cout << "  Sample sorted (first 10): ";
    for (int i = 0; i < 10; i++)
        cout << sortedBooks[i] << (i < 9 ? ", " : "\n");


    // ── 4. BINARY SEARCH (on sorted array) ───
    printHeader("STEP 4: Binary Search on Sorted Array");

    CaseSet bs = runBinarySearchCases(sortedBooks);

    int midKey = sortedBooks[DATASET_SIZE / 2];
    int avgKey = sortedBooks[DATASET_SIZE * 3 / 4];

    cout << "\n  --- Best Case (exact midpoint element) ---\n";
    printSearchDetail("Binary Search", "Best", midKey, bs.best);
    traceBinarySearch(sortedBooks, midKey);

    cout << "\n  --- Average Case (3/4 position element) ---\n";
    printSearchDetail("Binary Search", "Average", avgKey, bs.average);
    traceBinarySearch(sortedBooks, avgKey);

    cout << "\n  --- Worst Case (element NOT in array) ---\n";
    printSearchDetail("Binary Search", "Worst", -1, bs.worst);
    traceBinarySearch(sortedBooks, -1);


    // ── 5. CUSTOM SEARCH ─────────────────────
    printHeader("STEP 5: Custom Search (Enter Your Own Key)");
    cout << "  Enter an accession number to search: ";
    int userKey;
    cin >> userKey;

    cout << "\n  Running Linear Search...\n";
    SearchResult userLS = linearSearch(unsortedBooks, userKey);
    printSearchDetail("Linear Search", "Custom", userKey, userLS);

    cout << "\n  Running Binary Search (on sorted array)...\n";
    SearchResult userBS = binarySearch(sortedBooks, userKey);
    printSearchDetail("Binary Search", "Custom", userKey, userBS);


    // ── 6. FINAL ANALYSIS ────────────────────
    printAnalysis(ls, bs, sortTimeMs);

    return 0;
}
