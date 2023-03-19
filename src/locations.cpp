#include "locations.h"

LocationDefinition LocationDefinition::get_def(LocationId id) {
    auto result = std::find_if(LOCATION_DEFINITIONS.begin(), LOCATION_DEFINITIONS.end(), [=](const LocationDefinition &def) {
        return def.id == id;
    });

    if (result == LOCATION_DEFINITIONS.end()) {
        bugcheck(LocationDefinitionLookupMiss, id);
    }

    return *result;
}

LocationDefinition LocationDefinition::get_def(const QString &name_or_code) {
    auto result = std::find_if(LOCATION_DEFINITIONS.begin(), LOCATION_DEFINITIONS.end(), [&](const LocationDefinition &def) {
        return def.internal_name == name_or_code || def.two_letter_code == name_or_code;
    });

    if (result == LOCATION_DEFINITIONS.end()) {
        bugcheck(LocationDefinitionLookupMiss, name_or_code);
    }

    return *result;
}
