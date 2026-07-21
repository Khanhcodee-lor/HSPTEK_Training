#ifndef JOURNAL_HPP
#define JOURNAL_HPP

#include "Document.hpp"
#include <iostream>
#include <string>

#include <iomanip>

class Journal : public Document {

private:
    int issueNumber;
    int month;
public:
    Journal(const std::string& id, const std::string& title, const std::string& publisher, int copies, int issueNumber, int month)
        : Document(id, title, publisher, copies), issueNumber(issueNumber), month(month) {}
    
    void display() const override {
        std::cout << "========================================" << std::endl;
        std::cout << "  [ JOURNAL ]" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        Document::display();
        std::cout << "  Issue No:    " << issueNumber << std::endl;
        std::cout << "  Month:       " << month << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
    void save(std::ostream& os) const override {
        os << "JOURNAL " << std::quoted(id) << " " << std::quoted(title) << " " << std::quoted(publisher) << " " << copies << " " << issueNumber << " " << month << "\n";
    }
    
    ~Journal(){};
};

#endif