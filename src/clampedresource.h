#pragma once

#include "types.h"
#include "hooks.h"
#include "serialize.h"

class ClampedResource : public Serializable {
public:
    ClampedResource(AorInt amount, AorInt max, HookType calc_max_hook);

    AorInt amount() const;

    template <typename HookCarrier> AorInt max(HookCarrier *carrier) const {
        AorInt max = m_max;
        carrier->call_hooks(m_calc_max_hook, { &max });
        return max;
    }

    template <typename HookCarrier> void add(AorInt amount, HookCarrier *carrier) {
        set(m_amount + amount, carrier);
    }

    template <typename HookCarrier> void set(AorInt amount, HookCarrier *carrier) {
        m_amount = amount;

        if (m_amount < 0) {
            m_amount = 0;
        } else if (m_amount > max(carrier)) {
            m_amount = max(carrier);
        }
    }

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);

private:
    AorInt m_amount;

    // Can be transient?
    AorInt m_max;
    HookType m_calc_max_hook;
};

// Convenience type to instantiate ClampedResource with so that call_hooks
// will dispatch to all characters
struct AllCharacters {
    void call_hooks(HookType type, const HookPayload &payload);

    static AllCharacters instance;
};

