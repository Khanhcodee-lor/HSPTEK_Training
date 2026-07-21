#ifndef BOOK_HPP
#define BOOK_HPP

#include "Document.hpp"
#include <iostream>
#include <string>

#include <iomanip>

class Book: public Document {
private:
    std::string author;
    int pages;
public: 
    Book(const std::string& id, const std::string& title, const std::string& publisher, int copies, const std::string& author, const int pages)
        : Document(id, title, publisher, copies), author(author), pages(pages) {}
    
    void display() const override {
        std::cout << "========================================" << std::endl;
        std::cout << "  [ BOOK ]" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        Document::display();
        std::cout << "  Author:      " << author << std::endl;
        std::cout << "  Pages:       " << pages << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
    void save(std::ostream& os) const override {
        os << "BOOK " << std::quoted(id) << " " << std::quoted(title) << " " << std::quoted(publisher) << " " << std::quoted(author) << " " << copies << " " << pages << "\n";
    }
    
    ~Book(){};
};

#endif