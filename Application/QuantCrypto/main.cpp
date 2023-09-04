#include <iostream>
#include <filesystem>
#include <fstream>

#include "BaseNodeLoader.hpp"
#include "ThreadUtil.hpp"

#include <nlohmann/json.hpp>

using namespace Util::DynamicLoader;
using namespace Util::Thread;

int main(int argc, const char **argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config.json>" << std::endl;
        return 1;
    }
    auto configPath = std::filesystem::path(argv[1]);
    configPath = std::filesystem::absolute(configPath);
    std::cout << "Config path: " << configPath << std::endl;

    std::ifstream ifs(configPath.string());
    nlohmann::json config = nlohmann::json::parse(ifs);

    ThreadUtil::setThreadName("MainThread");

    const auto &nodeArray = config["nodes"];
    std::vector<BaseNodeLoader> loaders;
    for (const auto &node : nodeArray) {
        loaders.emplace_back(node);
    }

    for (auto &loader : loaders) {
        loader.getNode()->start();
    }

    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}