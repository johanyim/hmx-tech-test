#include "FxTradeLoader.h"
#include "LoaderUtils.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

FxTrade* FxTradeLoader::createTradeFromLine(std::string line)
{
    std::istringstream record_stream(line);
    std::string item;

    // using a custom split function
    std::vector<std::string> items = split(line, separator);

    if (items.size() < 7) {
        throw std::runtime_error("Invalid line format");
    }

    // missing argument
    FxTrade* trade = new FxTrade(items[6], items[0]);

    std::tm tm = {};
    std::istringstream dateStream(items[1]);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    trade->setTradeDate(timePoint);

    trade->setInstrument(items[2] + items[3]);
    trade->setCounterparty(items[7]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));

    std::tm vdBuf = {};
    std::istringstream valueDateStream(items[6]);
    valueDateStream >> std::get_time(&vdBuf, "%Y-%m-%d");

    auto valueDate
        = std::chrono::system_clock::from_time_t(std::mktime(&vdBuf));

    trade->setValueDate(valueDate);

    return trade;
}

void FxTradeLoader::loadTradesFromFile(
    std::string filename, FxTradeList& tradeList)
{
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }

    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // TODO: skipping lines might need a conditional?
    int lineCount = 0;
    std::string line;
    while (std::getline(stream, line)) {
        // if (lineCount < 2) {
        if (lineCount < 2 || lineCount > 5) {
        } else {
            tradeList.add(createTradeFromLine(line));
        }
        lineCount++;
    }
}

std::vector<ITrade*> FxTradeLoader::loadTrades()
{
    FxTradeList tradeList;
    loadTradesFromFile(dataFile_, tradeList);

    std::vector<ITrade*> result;
    for (size_t i = 0; i < tradeList.size(); ++i) {
        result.push_back(tradeList[i]);
    }
    return result;
}

std::string FxTradeLoader::getDataFile() const { return dataFile_; }

void FxTradeLoader::setDataFile(const std::string& file) { dataFile_ = file; }
