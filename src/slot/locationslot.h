#pragma once

#include "slot.h"

class LocationSlot : public Slot {
public:
    LocationSlot(const LocationDefinition &def, QWidget *parent);

    virtual QPixmap pixmap() override;
    virtual bool do_hovering() override;
    virtual std::optional<TooltipInfo> tooltip_info() override;

    virtual void on_left_click(QMouseEvent *event) override;
    virtual void refresh() override;

private:
    QString location_description();
    QString location_subtext();

    LocationDefinition m_location_def;
};
