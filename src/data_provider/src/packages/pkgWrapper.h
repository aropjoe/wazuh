/*
 * Wazuh SYSINFO
 * Copyright (C) 2015-2020, Wazuh Inc.
 * December 14, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#ifndef _PKG_WRAPPER_H
#define _PKG_WRAPPER_H

#include <fstream>
#include "stringHelper.h"
#include "ipackageWrapper.h"
#include "sharedDefs.h"

const std::string APP_INFO_PATH{"Contents/Info.plist"};

class PKGWrapper final : public IPackageWrapper
{
public:
    explicit PKGWrapper(const PackageContext& ctx)
      : m_name{UNKNOWN_VALUE}
      , m_version{UNKNOWN_VALUE}
      , m_groups{UNKNOWN_VALUE}
      , m_description{UNKNOWN_VALUE}
      , m_architecture{UNKNOWN_VALUE}
      , m_format{"pkg"}
      , m_osPatch{UNKNOWN_VALUE}
    {
        getPkgData(ctx.filePath+ "/" + ctx.package + "/" + APP_INFO_PATH);
    }

    ~PKGWrapper() = default;

    std::string name() const override
    {
        return m_name;
    }
    std::string version() const override
    {
        return m_version;
    }
    std::string groups() const override
    {
        return m_groups;
    }
    std::string description() const override
    {
        return m_description;
    }
    std::string architecture() const override
    {
        return m_architecture;
    }
    std::string format() const override
    {
        return m_format;
    }
    std::string osPatch() const override
    {
        return m_osPatch;
    }

private:
    void getPkgData(const std::string& filePath)
    {
        std::fstream file {filePath, std::ios_base::in};
        static const auto getValueFnc
        {
            [](const std::string& val)
            {
                const auto start{val.find(">")};
                const auto end{val.rfind("<")};
                return val.substr(start+1, end - start -1);
            }
        };
        if (file.is_open())
        {
            std::string line;
            while(std::getline(file, line))
            {
                line = Utils::trim(line," \t");

                if (line == "<key>CFBundleName</key>" &&
                    std::getline(file, line))
                {
                    m_name = getValueFnc(line);
                }
                else if (line == "<key>CFBundleShortVersionString</key>" &&
                         std::getline(file, line))
                {
                    m_version = getValueFnc(line);
                }
                else if (line == "<key>LSApplicationCategoryType</key>" &&
                         std::getline(file, line))
                {
                    m_groups = getValueFnc(line);
                }
                else if (line == "<key>CFBundleIdentifier</key>" &&
                         std::getline(file, line))
                {
                    m_description = getValueFnc(line);
                }
            }
        }
    }

    std::string m_name;
    std::string m_version;
    std::string m_groups;
    std::string m_description;
    std::string m_architecture;
    std::string m_format;
    std::string m_osPatch;
};

#endif //_PKG_WRAPPER_H