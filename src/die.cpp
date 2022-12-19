#include "die.h"

QString bugcheck_error_string(FatalErrorType type) {
    switch (type) {
        case ItemCodeNonexistant: {
            return "ITEM_CODE_NONEXISTANT";
        }
        case UncaughtUnknownException: {
            return "UNTOLD_RUNIC_HORROR";
        }
        case ProductsForUnknownDomain: {
            return "PRODUCTS_FOR_UNKNOWN_DOMAIN";
        }
        case InjuriesForUnknownDomain: {
            return "INJURIES_FOR_UNKNOWN_DOMAIN";
        }
        case AssessmentForUnknownDomain: {
            return "ASSESSMENT_FOR_UNKNOWN_DOMAIN";
        }
        case NonEffectIntoEffectSlot: {
            return "NON_EFFECT_INTO_EFFECT_SLOT";
        }
        case EncyclopediaSlotSet: {
            return "ENCYCLOPEDIA_SLOT_SET";
        }
        case PortraitSlotGet: {
            return "PORTRAIT_SLOT_GET";
        }
        case PortraitSlotSet: {
            return "PORTRAIT_SLOT_SET";
        }
        case SaveInvalidHeader: {
            return "INVALID_HEADER_IN_SAVE_FILE";
        }
        case EmptyListSample: {
            return "SAMPLE_FROM_EMPTY_LIST";
        }
        case EmptyIdRef: {
            return "EMPTY_ID_REF_GET";
        }
        case ItemByIdLookupMiss: {
            return "ITEM_INSTANCE_BY_ID_LOOKUP_MISS";
        }
        case DefLookupMiss: {
            return "ITEM_DEF_LOOKUP_MISS";
        }
        case CodeLookupMiss: {
            return "ITEM_CODE_LOOKUP_MISS";
        }
        case OutOfRangeException: {
            return "OUT_OF_RANGE_EXCEPTION";
        }
        case DeserializationFailure: {
            return "DESERIALIZATION_FAILURE";
        }
        case CharacterByIdLookupMiss: {
            return "CHARACTER_BY_ID_LOOKUP_MISS";
        }
    }

    return "TOTAL_RUIN";
}
