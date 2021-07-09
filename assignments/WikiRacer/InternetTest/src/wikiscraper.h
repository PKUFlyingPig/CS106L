#ifndef WIKISCRAPER_H
#define WIKISCRAPER_H

#include <QtNetwork>
#include <string>
#include <unordered_map>

class WikiScraper {

public:
    WikiScraper();
    std::string getPageSource(const std::string& page_name);

private:
    QNetworkAccessManager manager;
    std::unordered_map<std::string, std::string> memo;
};

/* This fix so that we can initalize a QCoreApplication
 * object to use the async networking facilities for
 * the WikiScraper.
 *
 * In particular, we use a QEventLoop, which needs the existence
 * of a QCoreApplication instance in the program. See alternative
 * fix in commented out default constructor for WikiScraper().
 */
#undef main
#define main main(int argc, char** argv) { \
                QCoreApplication a(argc, argv); \
                extern int StudentMain(); \
                return StudentMain(); \
            } \
            int StudentMain


#endif // WIKISCRAPER_H


