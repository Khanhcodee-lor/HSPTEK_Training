#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <iomanip>

#include "Document.hpp"
#include "Book.hpp"
#include "Journal.hpp"

const std::string DATA_FILE = "library.txt";

void loadLibrary(const std::string& filename, std::vector<std::unique_ptr<Document>>& library, std::map<std::string, Document*>& search_map) {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    library.clear();
    search_map.clear();
    std::string cmd;
    while (file >> cmd) {
        if (cmd == "BOOK") {
            std::string id, title, publisher, author;
            int copies, pages;
            if (file >> std::quoted(id) >> std::quoted(title) >> std::quoted(publisher) >> std::quoted(author) >> copies >> pages) {
                auto book = std::make_unique<Book>(id, title, publisher, copies, author, pages);
                search_map[id] = book.get();
                library.push_back(std::move(book));
            }
        } else if (cmd == "JOURNAL") {
            std::string id, title, publisher;
            int copies, issueNumber, month;
            if (file >> std::quoted(id) >> std::quoted(title) >> std::quoted(publisher) >> copies >> issueNumber >> month) {
                auto journal = std::make_unique<Journal>(id, title, publisher, copies, issueNumber, month);
                search_map[id] = journal.get();
                library.push_back(std::move(journal));
            }
        }
    }
    file.close();
}

void saveLibrary(const std::string& filename, const std::vector<std::unique_ptr<Document>>& library) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    for (const auto& doc : library) {
        doc->save(file);
    }
    file.close();
}

int main() {
    std::vector<std::unique_ptr<Document>> library;
    std::map<std::string, Document*> search_map;
    std::string command;
    
    // Tự động tải dữ liệu từ file khi khởi động
    loadLibrary(DATA_FILE, library, search_map);
    
    while (std::cin >> command){
        if(command == "EXIT"){
            break;
        }
        else if (command == "BOOK"){
            std::string id, title, publisher, author;
            int copies, pages;

            std::cin >> std::quoted(id) >> std::quoted(title) >> std::quoted(publisher) >> std::quoted(author) >> copies >> pages;
            if (search_map.find(id) != search_map.end()) {
                std::cout << "Error: Document ID " << id << " already exists." << std::endl;
            } else {
                auto book = std::make_unique<Book>(id, title, publisher, copies, author, pages);
                search_map[id] = book.get();
                library.push_back(std::move(book));
            }
        }
        else if(command == "JOURNAL"){
            std::string id, title, publisher;
            int copies, issueNumber, month;
            std::cin >> std::quoted(id) >> std::quoted(title) >> std::quoted(publisher) >> copies >> issueNumber >> month;
            if (search_map.find(id) != search_map.end()) {
                std::cout << "Error: Document ID " << id << " already exists." << std::endl;
            } else {
                auto journal = std::make_unique<Journal>(id, title, publisher, copies, issueNumber, month);
                search_map[id] = journal.get();
                library.push_back(std::move(journal));
            }
        }
        else if (command == "SEARCH"){
            std::string id;
            std::cin >> std::quoted(id);
            auto it = search_map.find(id);
            if (it != search_map.end()) {
                it->second->display();
            } else {
                std::cout << "Document not found" << std::endl;
            }
        }
        else if (command == "DISPLAY"){
            for (const auto& doc : library){
                doc->display();
            }
        }
        else if (command == "SAVE"){
            saveLibrary(DATA_FILE, library);
            std::cout << "Library saved to " << DATA_FILE << std::endl;
        }
        else if (command == "LOAD"){
            loadLibrary(DATA_FILE, library, search_map);
            std::cout << "Library loaded from " << DATA_FILE << std::endl;
        }
    }

    // Tự động lưu dữ liệu vào file khi thoát
    saveLibrary(DATA_FILE, library);

    return 0;
}