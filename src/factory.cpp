//
// Created by micha on 19.01.2026.
//

#include "factory.hpp"
#include <stdexcept>
#include "nodes.hpp"
#include <sstream>


bool has_reachable_storehouse(const PackageSender* sender,std::map<const PackageSender*,NodeColor>& node_colors){
    if (node_colors[sender]==NodeColor::VERIFIED) {
        return true;
    }
    node_colors[sender]=NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Brak zdefiniowanych odbiorcow!");
    }
    bool has_neighbors = false;
    for (auto &pair : sender->receiver_preferences_.get_preferences()) {
        auto* receiver=pair.first;
        if (receiver->get_receiver_type()==ReceiverTypes::STOREHOUSE) {
            has_neighbors=true;
        }
        else if (receiver->get_receiver_type()==ReceiverTypes::WORKER) {
            auto worker_ptr = dynamic_cast<Worker*>(receiver);
            auto reworker_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (static_cast<const void*>(reworker_ptr) == static_cast<const void*>(sender)) {
                continue;
            }
            has_neighbors=true;
            if (node_colors[worker_ptr]==NodeColor::UNVISITED) {
                has_reachable_storehouse(worker_ptr, node_colors);
            }
        }
    }
    if (has_neighbors) {
        node_colors[sender]=NodeColor::VERIFIED;
        return true;
    }
    else {
        throw std::logic_error("Brak prawidlowych odbiorcow");
    }
}

bool Factory::is_consistent(){
    std::map<const PackageSender*, NodeColor> node_colors;

    for (auto &worker : workers) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    for (auto &ramp : ramps) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    try {
        for (auto &ramp : ramps) {
            has_reachable_storehouse (&ramp , node_colors);
            }
        }
    catch (const std::logic_error&) {
        return false;
    }
    return true;
};

void Factory::do_deliveries(Time t) {
    for (Ramp &r : ramps) {
        r.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (Worker &w : workers) {
        w.send_package();
    }
    for (Ramp &r : ramps) {
        r.send_package();
    }
}

void Factory::do_work(Time t) {
    for (Worker &w : workers) {
        w.do_work(t);
    }
}

ParsedLineData parse_line (std::string& line) {
    std::vector<std::string> tokens;
    std::string token;

    std::istringstream stream_line(line);
    char limiter = ' ';

    while (std::getline(stream_line, token , limiter)) {
        tokens.push_back(token);
    }

    ParsedLineData parsed_data;

    std::map<std::string, ElementType> element_types {
            {"LOADING_RAMP" , ElementType::RAMP},
            {"WORKER" , ElementType::WORKER},
            {"STOREHOUSE" , ElementType::STOREHOUSE},
            {"LINK" , ElementType::LINK}};

    try {
        parsed_data.element_type = element_types.at(tokens[0]);

        std::for_each(std::next(tokens.begin()), tokens.end(),[&parsed_data](const std::string& current_str) {
            std::istringstream stream_str (current_str);
            std::string str_;
            std::vector<std::string> tokens_;
            char limiter_ = '=';
            while (std::getline(stream_str , str_ , limiter_)) {
                tokens_.push_back(str_);
            }
            if (tokens_.size()==2){
                parsed_data.parameters [tokens_[0]] = tokens_[1];
            }
            else {
                throw std::invalid_argument("Invalid string structure!");
            }
        });
    }
    catch(std::out_of_range& v) {
        throw std::invalid_argument("Invalid data type:(");
    }
    return parsed_data;
}
