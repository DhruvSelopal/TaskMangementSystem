#include <uwebsockets/App.h>
#include<nlohmann/json.hpp>
#include <stdio.h>
#include "iostream"
#include<string>
#include<unordered_map>
#include"task.cpp"

int main(){
    std::unordered_map<int,Task *> db;
    int idCount = 100;
    std::cout << "Hello World";
}