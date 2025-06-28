#ifndef TASKMANAGMENT
#define TASKMANAGMENT

#include<iostream>
#include<string.h>
#include<uwebsockets/App.h>
#include<nlohmann/json.hpp>
#include"task.cpp"

class TaskMangment{
private:
    uWS::App * app;
public:

    TaskMangment(uWS::App * app):app(app){}

    void createTask(std::string routeHandler,std::unordered_map<int,Task *> &db,int &id){
        app->post(routeHandler,[&db,&id](uWS::HttpResponse<false> * res,uWS::HttpRequest * req){
            res->onAborted([](){
                std::cout << "create task Api call has been aborted" << std::endl;
            });
            res->onData([res,&db,&id,body=std::string{}](std::string_view chunk,bool isLast) mutable{
                body.append(chunk);
                if(isLast){
                    nlohmann::json j = nlohmann::json::parse(body);
                    Task* t = new Task(id);
                    if(j.contains("title")) t->title = j["title"];
                    if(j.contains("descryption")) t->descryption = j["descyption"];
                    if(j.contains("status")) t->status = j["status"];
                    db[id] = t;
                    id++;
                    res->writeStatus("200")->end("object created successfully");
                }
            });
        });
    }
    void getTask(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->get(routeHandler + "/:id",[&db](uWS::HttpResponse<false> * res,uWS::HttpRequest *req){
            res->onAborted([]{
                std::cout << "getTask api call has been aborted" << std::endl;
            });
            int id = std::stoi(std::string(req->getParameter(0)));
            nlohmann::json j;
            to_json(j,*(db[id]));
            res->end(j.dump());
        });
    }
};

#endif