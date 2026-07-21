#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <iostream>
#include <string>   

class Document {
protected:
    std::string id;
    std::string title;
    std::string publisher;
    int copies;

public: 
    Document(const std::string& id, const std::string& title, const std::string& publisher, int copies)
        : id(id), title(title), publisher(publisher), copies(copies) {}
    
    virtual void display() const {
        std::cout << "  ID:          " << id << std::endl;
        std::cout << "  Title:       " << title << std::endl;
        std::cout << "  Publisher:   " << publisher << std::endl;
        std::cout << "  Copies:      " << copies << std::endl;
    }

    std::string getId() const { return id; }
    
    virtual void save(std::ostream& os) const = 0;
    
    virtual ~Document(){};
};

#endif