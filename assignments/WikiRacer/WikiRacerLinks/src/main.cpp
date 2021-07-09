#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>

using std::cout;            using std::endl;
using std::string;          using std::unordered_set;

/*
 * Note that you have to pass in the file as a single string
 * into the findWikiLinks function!
 * So we have to turn the passed-in file into a single string...
 * does that sound familiar at all?
 */
unordered_set<string> findWikiLinks(const string& page_html) {
    // TODO: delete this return statement and implement the
    //       function!
    unordered_set<string> WikiLinks;
    const string link_start_pattern = "<a href=\"/wiki/";
    const string link_end_pattern = "\"";
    auto search_start = page_html.begin();
    auto search_end = page_html.end();
    while (search_start != search_end) {
        auto link_start = std::search(search_start, search_end, link_start_pattern.begin(), link_start_pattern.end());
        if (link_start == search_end) break;
        auto link_end = std::search(link_start + link_start_pattern.length(), search_end, link_end_pattern.begin(), link_end_pattern.end());
        auto page_name = string(link_start + link_start_pattern.length(), link_end);
        if (std::all_of(page_name.begin(), page_name.end(), [](char c){return c != ':' && c != '#';})) {
            WikiLinks.insert(page_name);
        }
        search_start = link_end + 2;
    }
    return WikiLinks;
}

int main() {
    /* Note if your file reading isn't working, please go to the
     * projects tab on the panel on the left, and in the run section,
     * uncheck the "Run in terminal" box and re-check it. This
     * should fix things.
     */

    string filename = "quokka.txt";
    cout << "Enter a file name: " << filename << endl;;
    // getline(std::cin, filename);
    string page_html;

    // TODO: Create a filestream from the filename, and convert it into
    //       a single string of data called page_html (declared above)

    // Write code here
    std::ifstream in(filename, std::ifstream::in);
    string partial_content;
    while (getline(in, partial_content)) {
        page_html += partial_content;
    }

    unordered_set<string> validLinks = findWikiLinks(page_html);
    // TODO: Print out the validLinks set above.
    //       You can print it out in any format; this is just for you
    //       to be able to compare your results with the [output] results
    //       under the /res folder.

    // Write code here
    for (auto& link : validLinks) {
        cout << link << endl;
    }

    return 0;
}
