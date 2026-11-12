#pragma once
#include "planova/common/IEventStore.h"
#include <filesystem>

class FileEventStore : public IEventStore
{
public:
    explicit FileEventStore(std::filesystem::path dir);
    void append(const Event& event) override;
    std::vector<Event> readAll() const override;
    void setDirectory(std::filesystem::path dir);

private:
    std::filesystem::path dir_;
};