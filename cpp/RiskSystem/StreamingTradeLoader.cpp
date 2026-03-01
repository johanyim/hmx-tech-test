#include "StreamingTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"
#include "Models/IPricingEngine.h"
#include "NameResolver.h"
#include "PricingConfigLoader.h"
#include <iostream>
#include <stdexcept>

std::vector<std::unique_ptr<ITradeLoader>>
StreamingTradeLoader::getTradeLoaders()
{
    std::vector<std::unique_ptr<ITradeLoader>> loaders;

    loaders.push_back(std::make_unique<BondTradeLoader>());
    loaders.back()->setDataFile("TradeData/BondTrades.dat");

    loaders.push_back(std::make_unique<FxTradeLoader>());
    loaders.back()->setDataFile("TradeData/FxTrades.dat");

    return loaders;
}

void StreamingTradeLoader::loadPricers()
{
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    // reset pricers
    pricers_.clear();

    for (const auto& configItem : pricerConfig) {

        std::string tradeType = configItem.getTradeType();
        std::string typeName = configItem.getTypeName();

        pricers_.emplace(tradeType, NameResolver::instance().create(typeName));
    }
}

StreamingTradeLoader::~StreamingTradeLoader() { }

// NOTE: ScalarResults(.h) implements IScalarResultReceiver implmeents iterator
void StreamingTradeLoader::loadAndPrice(IScalarResultReceiver* resultReceiver)
{

    // pricers_.end()->second.getTradeLoaders();

    // NOTE: reference SerialPricer.cpp
    // for (const auto& tradeContainer : tradeContainers) {
    //     for (ITrade* trade : tradeContainer) {
    //         std::string tradeType = trade->getTradeType();
    //         if (pricers_.find(tradeType) == pricers_.end()) {
    //             resultReceiver->addError(trade->getTradeId(),
    //                 "No Pricing Engines available for this trade type");
    //             continue;
    //         }
    //         IPricingEngine* pricer = pricers_[tradeType].get();
    //         pricer->price(trade, resultReceiver);
    //     }
    // }

    loadPricers();
    std::vector<std::unique_ptr<ITradeLoader>> loaders = getTradeLoaders();
    // std::vector<std::unique_ptr<IPricingEngine>> pricers = loadPricers();

    for (const auto& loader : loaders) {
        auto trades = loader->loadTrades();

        // std::string tradeType = configItem.getTradeType();
        // std::string typeName = configItem.getTypeName();
        // pricers_.emplace(tradeType,
        // NameResolver::instance().create(typeName));

        for (const auto& trade : trades) {
            std::string tradeType = trade->getTradeType();
            IPricingEngine* pricer = pricers_[tradeType].get();
            if (!pricer) {
                std::cerr << "No pricer was found for the tradeType: "
                          << tradeType << '\n';
                continue;
            }
            pricer->price(trade, resultReceiver);
        }
    }
}
