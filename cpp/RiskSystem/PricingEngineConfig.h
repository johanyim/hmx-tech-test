#ifndef PRICINGENGINECONFIG_H
#define PRICINGENGINECONFIG_H

#include <vector>

#include "PricingEngineConfigItem.h"

class PricingEngineConfig : public std::vector<PricingEngineConfigItem> {
public:
    PricingEngineConfig() = default;

    PricingEngineConfig(const std::vector<PricingEngineConfigItem>& items)
        : std::vector<PricingEngineConfigItem>(items)
    {
    }
};

#endif // PRICINGENGINECONFIG_H
