#include <iostream>
#include <string>
#include "./MyStack.hpp"

int main(){
    MyStack<int> s;
    std::string command;

    while (std::cin >> command){
        if (command == "PUSH"){
            int value;
            if (std::cin >> value){
                s.push(value);
            }
        } else if (command == "POP"){
            s.pop();
        } else if (command == "TOP"){
            if (!s.isEmpty()){
                std::cout << s.top() << std::endl;
            }
        } else if (command == "SIZE"){
            std::cout << s.size() << std::endl;
        } else if (command == "isEmpty"){
            std::cout << (s.isEmpty() ? "True" : "False") << std::endl;
        } else if (command == "PRINT"){
            s.print();
        }
    }
}
