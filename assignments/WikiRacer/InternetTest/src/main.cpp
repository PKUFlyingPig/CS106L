
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>

#include "wikiscraper.h"

using std::cout;    using std::cin;
using std::endl;    using std::string;

void waitForInput() {
    string msg = " Take screenshot, then press enter to continue: ";
    cout << string(msg.size(), '=') << endl;
    cout << msg << endl;
    cout << string(msg.size(), '=') << endl;
    string s;
    (void)std::getline(std::cin, s);
}

int main() {
       WikiScraper w;
       //waitForInput();
       cout <<   w.getPageSource("Strawberry") <<  endl;
       //waitForInput()
       cout << "========================" <<  endl;
       cout <<  w.getPageSource("Mathematics") <<  endl;
        //waitForInput();
       cout << "========================" <<  endl;
       cout <<  w.getPageSource("Stanford_University") <<  endl;
       //waitForInput();
       cout << "Done!" << endl;
       return 0;
}



