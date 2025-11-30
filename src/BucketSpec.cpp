#include "BucketSpec.h"
#include <cmath>

BucketSpec::Strategy BucketSpec::strategyFromString(const juce::String& str) {
    if (str.equalsIgnoreCase("ExplicitValues"))
        return Strategy::ExplicitValues;
    if (str.equalsIgnoreCase("Linear"))
        return Strategy::Linear;
    if (str.equalsIgnoreCase("Log"))
        return Strategy::Log;
    if (str.equalsIgnoreCase("EdgeAndCenter"))
        return Strategy::EdgeAndCenter;

    return Strategy::Linear; // default
}

std::vector<float> BucketSpec::generateValues() const {
    std::vector<float> result;

    switch (strategy) {
        case Strategy::ExplicitValues:
            result = values;
            break;

        case Strategy::Linear: {
            if (numBuckets <= 1) {
                result.push_back(min);
            } else {
                for (int i = 0; i < numBuckets; ++i) {
                    float t = (float)i / (float)(numBuckets - 1);
                    result.push_back(min + t * (max - min));
                }
            }
            break;
        }

        case Strategy::Log: {
            if (numBuckets <= 1) {
                result.push_back(min);
            } else {
                float logMin = std::log10(std::max(min, 1e-6f));
                float logMax = std::log10(std::max(max, 1e-6f));
                for (int i = 0; i < numBuckets; ++i) {
                    float t = (float)i / (float)(numBuckets - 1);
                    float logVal = logMin + t * (logMax - logMin);
                    result.push_back(std::pow(10.0f, logVal));
                }
            }
            break;
        }

        case Strategy::EdgeAndCenter: {
            result.push_back(min);
            result.push_back((min + max) / 2.0f);
            result.push_back(max);
            break;
        }
    }

    return result;
}
