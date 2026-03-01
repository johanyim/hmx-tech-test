#ifndef ITRADELOADER_H
#define ITRADELOADER_H

#include <functional>
#include <string>
#include <vector>

#include "../Models/ITrade.h"

class ITradeLoader {
public:
    virtual ~ITradeLoader() = default;
    virtual std::vector<ITrade*> loadTrades() = 0;
    // trade loader gains a new trait method, callback to process the function
    // virtual void streamTrades(std::function<void(const ITrade&)> onTrade) =
    // 0;
    virtual std::string getDataFile() const = 0;
    virtual void setDataFile(const std::string& file) = 0;
};

#endif // ITRADELOADER_H
