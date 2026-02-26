#include "SerialPricer.h"
#include "Pricers/CorpBondPricingEngine.h"
#include "Pricers/FxPricingEngine.h"
#include "Pricers/GovBondPricingEngine.h"
#include <memory>
#include <stdexcept>

// NOTE: No need forthis as the unique_ptr will automatically cleanup
// SerialPricer::~SerialPricer() { }

void SerialPricer::loadPricers()
{
#define REGISTER_PRICER(Type)                                                  \
    pricers_.emplace(                                                          \
        "HmxLabs.TechTest.Pricers." #Type, std::make_unique<Type>())

    REGISTER_PRICER(GovBondPricingEngine);
    REGISTER_PRICER(CorpBondPricingEngine);
    REGISTER_PRICER(FxPricingEngine);

    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    for (const auto& configItem : pricerConfig) {
        throw std::runtime_error("Not implemented");
    }
}

void SerialPricer::price(
    const std::vector<std::vector<ITrade*>>& tradeContainers,
    IScalarResultReceiver* resultReceiver)
{
    loadPricers();

    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            std::string tradeType = trade->getTradeType();
            if (pricers_.find(tradeType) == pricers_.end()) {
                resultReceiver->addError(trade->getTradeId(),
                    "No Pricing Engines available for this trade type");
                continue;
            }

            IPricingEngine* pricer = pricers_[tradeType].get();
            pricer->price(trade, resultReceiver);
        }
    }
}
