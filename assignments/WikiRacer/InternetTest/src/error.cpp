#include "error.h"
#include <iostream>

using std::cerr;    using std::endl;
using std::string;

void errorPrint(const std::string& msg, const std::string& leftDisp) {
    cerr << leftDisp << msg << endl;
}
