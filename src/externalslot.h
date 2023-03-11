#pragma once

#include "itemslot.h"

class ExternalSlot : public ItemSlot {};

class ToolSlot : public ExternalSlot {};

class PortraitSlot : public ExternalSlot {};
