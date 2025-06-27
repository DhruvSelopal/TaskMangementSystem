#ifndef TASK_CPP
#define TASK_CPP


#include<string>
#include<nlohmann/json.hpp>


struct Task {
    int id;
    std::string title;
    std::string description;
    std::string status; // e.g. "todo", "in-progress", "done"
    Task(int id):id(id),title(""),description(""),status(""){}
};

void to_json(nlohmann::json &j,Task & t){
    j = nlohmann::json{
        {"id",t.id},
        {"title",t.title},
        {"descryption",t.description},
        {"status",t.status}
    };
}

#endif