#include "die.h"

QString bugcheck_error_string(FatalErrorType type) {
    switch (type) {
        case ItemCodeNonexistant: {
            return "UNDEFINED_MATERIALIZATION";
        }
        case UncaughtUnknownException: {
            return "UNTOLD_RUNIC_HORROR";
        }
    }

    return "TOTAL_RUIN";
}
