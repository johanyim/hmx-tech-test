#include "SerialPricer.h"
#include "Models/IPricingEngine.h"
#include "Pricers/CorpBondPricingEngine.h"
#include "Pricers/FxPricingEngine.h"
#include "Pricers/GovBondPricingEngine.h"
#include <functional>
#include <memory>
#include <stdexcept>

// NOTE: No need forthis as the unique_ptr will automatically cleanup
// SerialPricer::~SerialPricer() { }

// singleton class name resolver
class NameResolver {
public:
    static NameResolver& instance()
    {
        static NameResolver resolver;
        return resolver;
    }

    // Lookup by string
    /// Can error if type name is not registered
    std::unique_ptr<IPricingEngine> create(const std::string& typeName)
    {
        auto it = resolver_.find(typeName);
        if (it != resolver_.end()) {
            // successful return map kv key
            return it->second();
        }

        // potentially std::bad_function_call may be a suitable error here?
        throw std::runtime_error(
            "NameResolver could not find engine with name: " + typeName);
    }

private:
    NameResolver()
    {
        // #define RESOLVE_NAME(Type)                                                     \
//     resolver_.emplace(                                                         \
//         "HmxLabs.TechTest.Pricers." #Type, std::make_shared<Type>())

#define RESOLVE_NAME(Type)                                                     \
    resolver_.emplace("HmxLabs.TechTest.Pricers." #Type,                       \
        []() { return std::make_unique<Type>(); })

        // expands to register names of engine
        RESOLVE_NAME(GovBondPricingEngine);
        RESOLVE_NAME(CorpBondPricingEngine);
        RESOLVE_NAME(FxPricingEngine);
    }

    // I considered returning shared_ptrs of instances so that each engine is
    // only ever instantiated once.
    // std::map<std::string, std::shared_ptr<IPricingEngine>> resolver_;

    std::map<std::string, std::function<std::unique_ptr<IPricingEngine>()>>
        resolver_;
};

void SerialPricer::loadPricers()
{
    // DISCUSSION (exercise 4): runtime type resolution is a tradeoff between
    // compile type safety (+ some runtime overhead) in favor of
    // flexibility/dynamic configuration. Additionally, refactoring becomes
    // slightly harder when working with strings instead of named identifiers,
    // the latter of which the compiler can assist with.
    //
    // I'm not sure what the exact use case of this system is - but if possible
    // I would vouch for a compile-time approach.

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
