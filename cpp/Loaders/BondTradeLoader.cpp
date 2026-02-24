#include "BondTradeLoader.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

//
//
#include <algorithm>
#include <cctype>

// TODO: maybe this cuold be in a utils file
static inline std::string trim(const std::string& s) {
    auto start = std::find_if_not(s.begin(), s.end(), ::isspace);
    auto end = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    if (start >= end) return "";
    return std::string(start, end);
}

BondTrade* BondTradeLoader::createTradeFromLine(std::string line) {
    std::vector<std::string> items;
    std::istringstream record_stream(line);
    std::string item;

    while (std::getline(record_stream, item, separator)) {
        // string fields get trimmed before pushed
        items.push_back(trim(item));
    }

    if (items.size() < 7) {
        throw std::runtime_error("Invalid line format");
    }

    // missing argument
    BondTrade* trade = new BondTrade(items[6], items[0]);

    std::tm tm = {};
    std::istringstream dateStream(items[1]);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    trade->setTradeDate(timePoint);

    trade->setInstrument(items[2]);
    trade->setCounterparty(items[3]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));

    return trade;
}

void BondTradeLoader::loadTradesFromFile(std::string filename,
                                         BondTradeList& tradeList) {
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }

    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    int lineCount = 0;
    std::string line;
    while (std::getline(stream, line)) {
        if (lineCount == 0) {
        } else {
            tradeList.add(createTradeFromLine(line));
        }
        lineCount++;
    }
}

std::vector<ITrade*> BondTradeLoader::loadTrades() {
    BondTradeList tradeList;
    loadTradesFromFile(dataFile_, tradeList);

    std::vector<ITrade*> result;
    for (size_t i = 0; i < tradeList.size(); ++i) {
        result.push_back(tradeList[i]);
    }
    return result;
}

std::string BondTradeLoader::getDataFile() const { return dataFile_; }

void BondTradeLoader::setDataFile(const std::string& file) { dataFile_ = file; }
