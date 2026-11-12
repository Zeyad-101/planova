#pragma once
#include <functional>
#include <string>
#include <utility>
#include <vector>

class ChangeBus {
public:
    using Fn = std::function<void(const std::string& eventType)>;
    int subscribe(Fn fn);
    void unsubscribe(int token);
    void publish(const std::string& eventType);

private:
    std::vector<std::pair<int, Fn>> subs_;
    int nextToken_ = 1;
};