#ifndef WIKISCRAPER_H
#define WIKISCRAPER_H

#include <QtNetwork>
#include <string>
#include <unordered_map>
#include <unordered_set>

class WikiScraper {

public:
    WikiScraper();

    /*
     * This method takes the page name of a Wikipedia page and
     * returns an unordered_set containing all the links on this page.
     */
    std::unordered_set<std::string> getLinkSet(const std::string& page_name);


private:
    QNetworkAccessManager manager;
    std::string getPageSource(const std::string& page_name);
    std::unordered_map<std::string, std::string> page_cache;
    std::unordered_map<std::string, std::unordered_set<std::string>> linkset_cache;
};


/* This fix so that we can initalize a QCoreApplication
 * object to use the async networking facilities for
 * the WikiScraper.
 *
 * In particular, we use a QEventLoop, which needs the existence
 * of a QCoreApplication instance in the program.
 */
#undef main
#define main main(int argc, char** argv) { \
                QCoreApplication a(argc, argv); \
                extern int StudentMain(); \
                return StudentMain(); \
            } \
            int StudentMain


#endif // WIKISCRAPER_H


