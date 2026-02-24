#include "BondTradeLoaderTests.cpp"
#include "FxTradeLoaderTests.cpp"
#include "PricingConfigLoaderTests.cpp"
#include "PricingEngineTests.cpp"
#include "ScalarResultsTests.cpp"
#include "TestFramework.h"

int main()
{
    // TestRunner::runAll();

    FxTradeLoader loader;
    loader.setDataFile("Loaders/TradeData/FxTrades.dat");
    auto trades = loader.loadTrades();
    for (auto t : trades) {
        std::cout << "start 12345 '" << t->getTradeId() << "' end 12345"
                  << std::endl;
    }
    return 0;
}
