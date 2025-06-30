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
    static void parseQuery(std::string_view sv,std::unordered_map<std::string,std::string> &myMap){
        bool first = true;
        std::string key,str = "";
        for(char ch : sv){
            if(ch == '='){
                key = str;
                str = "";
            }
            else if(ch == '&'){
                myMap[key] = str;
                str = "";
            }
            else str += ch;
        }
        myMap[key] = str;
    }
    static nlohmann::json wrap_task_by_id(const Task &t) {
    return {
        {std::to_string(t.id), {
            {"id", t.id},
            {"title", t.title},
            {"descryption", t.descryption},
            {"status", t.status}
        }}
    };
}
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
                    nlohmann::json j;
                    try{
                        j = nlohmann::json::parse(body);
                        Task* t = new Task(id);
                        if(j.contains("title")) t->title = j["title"];
                        if(j.contains("descryption")) t->descryption = j["descryption"];
                        if(j.contains("status")) t->status = j["status"];
                        db[id] = t;
                    } catch(std::exception &e){
                        std::cout << "standard error : " << e.what() << std::endl;
                    }
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
    void getAllTask(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->get(routeHandler,[&db](uWS::HttpResponse<false> *res,uWS::HttpRequest * req){
            res->onAborted([](){
                std::cout << "get all task was aborted";
            });
            nlohmann::json allTasks;
            for(std::pair<const int,Task *> &p : db){
                allTasks.update(TaskMangment::wrap_task_by_id(*(p.second)));
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
                    if(j.contains("descryption")) t->descryption = j["descryption"];
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
    void filterByStatus(std::string routeHandler,std::unordered_map<int,Task *> &db){
        app->get(routeHandler,[&db](uWS::HttpResponse<false> *res,uWS::HttpRequest * req){
            res->onAborted([](){
                std::cout << "filter by status was aborted";
            });
            std::unordered_map<std::string,std::string> queries;
            TaskMangment::parseQuery(req->getQuery(),queries);
            nlohmann::json j;
            std::unordered_map<int, Task*>::iterator task = db.begin();
            to_json(j,*(task->second));
            for(std::pair<std::string,std::string> query: queries){
                    if(!(j.contains(query.first))){
                        res->end("Invalid query key");
                        return;
                    }
            }
            nlohmann::json finalJson;
            for(;task != db.end();task++){
                to_json(j,*(task->second));
                bool filtered = true;
                for(std::pair<const std::string,std::string> &query : queries){
                    if(j[query.first] != query.second){
                        filtered = false;
                        break;
                    }
                }
                if(filtered){
                    to_json(j,*(task->second));
                    finalJson.update(TaskMangment::wrap_task_by_id(*(task->second)));
                }
            }
            res->end(finalJson.dump());
        });
    }
    void run(int portnumber){
        app->listen(portnumber,[portnumber](auto * token ){
            if(token){
                std::cout << "server listening on" << portnumber << std::endl;
            }
        });
        app->run();
    }
};

#endif