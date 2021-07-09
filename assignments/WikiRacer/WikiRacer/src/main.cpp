#include <iostream>     // for cout, cin
#include <fstream>      // for ifstream
#include <sstream>      // for stringstream
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include "wikiscraper.h"

using std::cout;            using std::endl;
using std::ifstream;        using std::stringstream;
using std::string;          using std::vector;
using std::priority_queue;  using std::unordered_map;
using std::unordered_set;   using std::cin;

/*
 * This is the function you will be implementing. It takes
 * two string representing the names of a start_page and
 * end_page and is supposed to return a ladder, represented
 * as a vector<string>, of links that can be followed from
 * start_page to get to the end_page.
 *
 * For the purposes of this algorithm, the "name" of a Wikipedia
 * page is what shows at the end of the URL when you visit that page
 * in your web browser. For ex. the name of the Stanford University
 * Wikipedia page is "Stanford_University" since the URL that shows
 * in your browser when you visit this page is:
 *
 *       https://en.wikipedia.org/wiki/Stanford_University
 */
vector<string> findWikiLadder(const string& start_page, const string& end_page) {
    // TODO: 1. Once your file reading is working, replace the below
    //          return statement with "return {};".
    //       2. Make sure that you've copied over your findWikiLinks method
    //          from Part A into wikiscraper.cpp.
    //       3. Finally, implement this function per Part B in the handout!
    //
    WikiScraper scraper;
    unordered_set<string> visited_pages;
    auto target_set = scraper.getLinkSet(end_page);
    auto cmpFn = [&](vector<string> ladder1, vector<string>ladder2) {
        auto page1_links = scraper.getLinkSet(ladder1.back());
        auto page2_links = scraper.getLinkSet(ladder2.back());
        int num1 = 0, num2 = 0;
        for (auto& s : target_set) {
            if (page1_links.find(s) != page1_links.end()) {
                num1 ++;
            }
            if (page2_links.find(s) != page2_links.end()) {
                num2 ++;
            }
        }
        return num1 < num2;
    };

    std::priority_queue<vector<string>, vector<vector<string>>,
            decltype (cmpFn)> ladderQueue(cmpFn);
    ladderQueue.push(vector<string>{start_page});
    visited_pages.insert(start_page);

    while (!ladderQueue.empty()) {
        auto partial_ladder = ladderQueue.top();
        ladderQueue.pop();
        auto current_links = scraper.getLinkSet(partial_ladder.back());
        if (current_links.find(end_page) != current_links.end()) {
            partial_ladder.push_back(end_page);
            return partial_ladder;
        }
        for (auto& link : current_links) {
            if (visited_pages.find(link) != visited_pages.end()) {
                continue;
            }
            vector<string> new_ladder(partial_ladder);
            new_ladder.push_back(link);
            visited_pages.insert(link);
            ladderQueue.push(new_ladder);
        }
    }
    return vector<string>{};
}

int main() {
    /* Container to store the found ladders in */
    vector<vector<string>> outputLadders;

    string filename = "input-big.txt";
    cout << "Enter a file name: " << filename << endl;
    //getline(cin, filename);

    // TODO: Create a filestream from the filename.
    //       For each pair {start_page, end_page} in the input file,
    //       retrieve the result of findWikiLadder(start_page, end_page)
    //       and append that vector to outputLadders.

    // Write code here
    ifstream in(filename, ifstream::in);
    int pair_nums;
    string start_page, end_page;
    in >> pair_nums;
    for (int i = 0; i < pair_nums; i++) {
        in >> start_page >> end_page;
        outputLadders.push_back(findWikiLadder(start_page, end_page));
    }

    /*
     * Print out all ladders in outputLadders.
     * We've already implemented this for you!
     */
    for (auto& ladder : outputLadders) {
        if(ladder.empty()) {
            cout << "No ladder found!" << endl;
        } else {
            cout << "Ladder found:" << endl;
            cout << "\t" << "{";

            std::copy(ladder.begin(), ladder.end() - 1,
                      std::ostream_iterator<string>(cout, ", "));
            /*
             * The above is an alternate way to print to cout using the
             * STL algorithms library and iterators. This is equivalent to:
             *    for (size_t i = 0; i < ladder.size() - 1; ++i) {
             *        cout << ladder[i] << ", ";
             *    }
             */
            cout << ladder.back() << "}" << endl;
        }
    }
    return 0;
}




