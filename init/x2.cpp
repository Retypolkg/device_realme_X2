/*
 * Copyright (C) 2020 LineageOS Project
 * Copyright (C) 2020 Harshit Jain
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vector>
#include <string>
#include <fstream>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/properties.h>
#include <android-base/logging.h>

#include "property_service.h"
#include "vendor_init.h"

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "vendor.",
};

bool isCN()
{
    // Get region
    std::ifstream infile("/proc/oppoVersion/region");
    std::string region;
    bool ret = false;
    getline(infile, region);
    if (!region.compare("China"))
        ret = true;
    return ret;
}

bool hasNFC()
{
    // Check NFC
    std::ifstream infile("/proc/touchpanel/NFC_CHECK");
    std::string check;
    bool ret = false;
    getline(infile, check);
    if (!check.compare("SUPPORTED"))
        ret = true;
    return ret;
}

// Make life easier
bool isRMX1993()
{
    return (!isCN() && hasNFC());
}

void property_override(char const prop[], char const value[], bool add = true)
{
    prop_info *pi;
    pi = (prop_info *)__system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

// Decommonise nfc properties
void load_nfc_props()
{
    property_override("persist.sys.nfc.disPowerSave", "1");
    property_override("persist.sys.nfc.default", "on");
    property_override("persist.sys.nfc.aid_overflow", "true");
    property_override("ro.product.cuptsm", "OPPO|ESE|01|02");
    property_override("persist.sys.nfc.antenna_area", "bottom");
}

void setRMX(const unsigned int variant)
{
    // Load NFC properties only on RMX199{1:3}
    if (variant == 2 || variant == 0)
        load_nfc_props();
}

void vendor_load_properties()
{
    if (isRMX1993())
    {
        setRMX(2); //RMX1993
    }
    else if (!isCN())
    {
        setRMX(1); //RMX1992
    }
    else
    {
        setRMX(0); //RMX1991
    }
}
