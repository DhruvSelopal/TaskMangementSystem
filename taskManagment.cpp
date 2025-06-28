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
            to_json(j,*(db[id]),1);
            res->end(j.dump());
        });
    }
    void getAllTask(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->get(routeHandler,[&db](uWS::HttpResponse<false> *res,uWS::HttpRequest * req){
            res->onAborted([](){
                std::cout << "get all task was aborted";
            });
            nlohmann::json allTasks;
            nlohmann::json task;
            for(std::pair<const int,Task *> &p : db){
                to_json(task,*(p.second));
                allTasks.update(task);
            }
            res->end(allTasks.dump());
        });
    }
    void updateTask(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->put(routeHandler+"/:id",[&db](uWS::HttpResponse<false> * res,uWS::HttpRequest * req){
            res->onAborted([]{
                std::cout << "update task was aborted" << std::endl;
            });
            int id = std::stoi(std::string(req->getParameter(0)));
            res->onData([res,&db,id,body=std::string{}](std::string_view chunk,bool isLast) mutable{
                body.append(chunk);
                if(isLast){
                    nlohmann::json j = nlohmann::json::parse(body);
                    Task* t = new Task(id);
                    if(j.contains("title")) t->title = j["title"];
                    if(j.contains("descryption")) t->descryption = j["descyption"];
                    if(j.contains("status")) t->status = j["status"];
                    delete(db[id]);
                    db[id] = t;
                    res->writeStatus("200")->end("object updated successfully");
                }
            });
        });
    }
    void deleteTask(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->del(routeHandler + "/:id",[&db](uWS::HttpResponse<false> * res,uWS::HttpRequest *req){
            res->onAborted([]{
                std::cout << "delete api call has been aborted" << std::endl;
            });
            int id = std::stoi(std::string(req->getParameter(0)));
            delete(db[id]);
            db.erase(id);
            res->writeStatus("200 ok")->end("Deleted succssfully");
        });
    }
    
};

#endif