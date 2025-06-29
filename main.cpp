#include <uwebsockets/App.h>
#include<nlohmann/json.hpp>
#include <stdio.h>
#include "iostream"
#include<string>
#include<unordered_map>
#include"task.cpp"
#include"taskManagment.cpp"

int main(){
    std::unordered_map<int,Task *> db;
    int idCount = 100;
    uWS::App * app  = new uWS::App();
    TaskMangment t(app);
    t.createTask("/main",db,idCount);
    t.deleteTask("/main",db);
    t.filterByStatus("/search",db);
    t.getAllTask("/main/tasks",db);
    t.getTask("/main",db);
    t.updateTask("/main",db);
    t.run(9001);
    
}