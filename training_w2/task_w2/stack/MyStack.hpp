#ifndef MY_STACK_HPP
#define MY_STACK_HPP

#include <stdexcept>
#include <vector>
#include <iostream>

template <typename T>
class MyStack {
private:
    std::vector<T> data;
public:
    MyStack() {};
    ~MyStack(){};
    void push(const T& value){
        data.push_back(value);
    }
    void pop(){
        if(data.empty()){
            std::cout << "Error: Stack empty" << std::endl;
        }else {
            data.pop_back();
        }
    }
    T& top()  { 
        if(data.empty()){
            throw std::runtime_error("Error: Stack empty");
        } else {
            return data.back();
        }
     }
    bool isEmpty() const {
        return data.empty();
    }
    int size() const{
        return data.size();
    }
    void print() const {
        std::cout << "Stack elements: ";
        for (const T& item : data){
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};

#endif