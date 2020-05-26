#include <any>
#include <fstream>
#include <iomanip>
#include "API/ARK/Ark.h"
#include "config.hpp"

const json Config::Default = {
    {"Admins", {
        {"DeadDinoDragging", true},
        {"DeadDinoInventory", true},
        {"DeadPlayerDragging", true},
        {"DeadPlayerInventory", true},
        {"OfflinePlayerDragging", true},
        {"OfflinePlayerInventory", true},
        {"TranqdPlayerDragging", true},
        {"TranqdPlayerInventory", true}
    }},
    {"CrashGuard", {
        {"Enable", true},
        {"PermanentImmunity", false},
        {"OfflineImmunitySeconds", 1200},
        {"ReconnectImmunitySeconds", 300}
    }},
    {"Players", {
        {"DeadDinoDragging", "tribe"},
        {"DeadDinoInventory", "tribe"},
        {"DeadPlayerDragging", "tribe"},
        {"DeadPlayerInventory", "tribe"},
        {"OfflinePlayerDragging", "tribe"},
        {"OfflinePlayerInventory", "tribe"},
        {"TranqdPlayerDragging", "tribe"},
        {"TranqdPlayerInventory", "tribe"}
    }}
};

json Config::Impl = Config::Default;

bool Config::Load()
{
    ArkApi::GetCommands().AddConsoleCommand("Bodyguard.ReloadConfig", &Config::Cmd_ReloadConfig);
    ArkApi::GetCommands().AddConsoleCommand("Bodyguard.WriteDefaultConfigToFile", &Config::Cmd_WriteDefaultConfigToFile);

    ReadFromFile();
    
    return true;
}

bool Config::Unload()
{
    bool result = true;
    bool success;

    success = ArkApi::GetCommands().RemoveConsoleCommand("Bodyguard.ReloadConfig");
    
    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to remove console command: Bodyguard.ReloadConfig", __func__, __LINE__);
        result = false;
    }
    
    success = ArkApi::GetCommands().RemoveConsoleCommand("Bodyguard.WriteDefaultConfigToFile");

    if(success == false) {
        Log::GetLog()->error("{}:{}: failed to remove console command: Bodyguard.WriteDefaultConfigToFile", __func__, __LINE__);
        result = false;
    }

    return result;
}

Config::Var Config::GetAdminVarForDragging(const CharacterType& type)
{
    switch(type) {
    case CharacterType::DeadDino:
        return Var::Admins_DeadDinoDragging;

    case CharacterType::DeadPlayer:
        return Var::Admins_DeadPlayerDragging;

    case CharacterType::OfflinePlayer:
        return Var::Admins_OfflinePlayerDragging;

    case CharacterType::TranqdPlayer:
        return Var::Admins_TranqdPlayerDragging;

    default:
        // C4715
        return std::any_cast<Config::Var>((Config::Var*) nullptr);
    }
}

Config::Var Config::GetAdminVarForInventory(const CharacterType& type)
{
    switch(type) {
    case CharacterType::DeadDino:
        return Var::Admins_DeadDinoInventory;

    case CharacterType::DeadPlayer:
        return Var::Admins_DeadPlayerInventory;

    case CharacterType::OfflinePlayer:
        return Var::Admins_OfflinePlayerInventory;

    case CharacterType::TranqdPlayer:
        return Var::Admins_TranqdPlayerInventory;

    default:
        // C4715
        return std::any_cast<Config::Var>((Config::Var*) nullptr);
    }
}

Config::Var Config::GetVarForDragging(const CharacterType& type)
{
    switch(type) {
    case CharacterType::DeadDino:
        return Var::Players_DeadDinoDragging;

    case CharacterType::DeadPlayer:
        return Var::Players_DeadPlayerDragging;

    case CharacterType::OfflinePlayer:
        return Var::Players_OfflinePlayerDragging;

    case CharacterType::TranqdPlayer:
        return Var::Players_TranqdPlayerDragging;

    default:
        // C4715
        return std::any_cast<Config::Var>((Config::Var*) nullptr);
    }
}

Config::Var Config::GetVarForInventory(const CharacterType& type)
{
    switch(type) {
    case CharacterType::DeadDino:
        return Var::Players_DeadDinoInventory;

    case CharacterType::DeadPlayer:
        return Var::Players_DeadPlayerInventory;

    case CharacterType::OfflinePlayer:
        return Var::Players_OfflinePlayerInventory;

    case CharacterType::TranqdPlayer:
        return Var::Players_TranqdPlayerInventory;

    default:
        // C4715
        return std::any_cast<Config::Var>((Config::Var*) nullptr);
    }
}

std::optional<AccessLevel> Config::GetAccessLevelForVar(Config::Var var)
{
    std::string var_str;

    switch(var) {
    case Var::Players_DeadDinoDragging:
        var_str = "DeadDinoDragging";
        break;

    case Var::Players_DeadDinoInventory:
        var_str = "DeadDinoInventory";
        break;

    case Var::Players_DeadPlayerDragging:
        var_str = "DeadPlayerDragging";
        break;

    case Var::Players_DeadPlayerInventory:
        var_str = "DeadPlayerInventory";
        break;

    case Var::Players_OfflinePlayerDragging:
        var_str = "OfflinePlayerDragging";
        break;

    case Var::Players_OfflinePlayerInventory:
        var_str = "OfflinePlayerInventory";
        break;

    case Var::Players_TranqdPlayerDragging:
        var_str = "TranqdPlayerDragging";
        break;

    case Var::Players_TranqdPlayerInventory:
        var_str = "TranqdPlayerInventory";
        break;

    default:
        Log::GetLog()->error("{}:{}: invalid config var", __func__, __LINE__);
        return std::nullopt;
    }

    const auto access_str = Config::Impl["Players"][var_str].get<std::string>();
    const auto access_level_opt = GetAccessLevelFromString(access_str);

    if(access_level_opt.has_value() == false) {
        Log::GetLog()->error("{}:{}: failed to translate from '{}' to access level", __func__, __LINE__, access_str);
    }

    return access_level_opt;
}

std::optional<bool> Config::GetBoolForAdminVar(Config::Var var)
{
    std::string var_str;

    switch(var) {
    case Var::Admins_DeadDinoDragging:
        var_str = "DeadDinoDragging";
        break;

    case Var::Admins_DeadDinoInventory:
        var_str = "DeadDinoInventory";
        break;

    case Var::Admins_DeadPlayerDragging:
        var_str = "DeadPlayerDragging";
        break;

    case Var::Admins_DeadPlayerInventory:
        var_str = "DeadPlayerInventory";
        break;

    case Var::Admins_OfflinePlayerDragging:
        var_str = "OfflinePlayerDragging";
        break;

    case Var::Admins_OfflinePlayerInventory:
        var_str = "OfflinePlayerInventory";
        break;

    case Var::Admins_TranqdPlayerDragging:
        var_str = "TranqdPlayerDragging";
        break;

    case Var::Admins_TranqdPlayerInventory:
        var_str = "TranqdPlayerInventory";
        break;

    default:
        Log::GetLog()->error("{}:{}: invalid config var", __func__, __LINE__);
        return std::nullopt;
    }

    return Config::Impl["Admins"][var_str].get<bool>();
}

std::optional<bool> Config::GetBoolForCrashGuardVar(Config::Var var)
{
    std::string var_str;

    switch(var) {
    case Var::CrashGuard_Enable:
        var_str = "Enable";
        break;

    case Var::CrashGuard_PermanentImmunity:
        var_str = "PermanentImmunity";
        break;

    default:
        Log::GetLog()->error("{}:{}: invalid config var", __func__, __LINE__);
        return std::nullopt;
    }

    return Config::Impl["CrashGuard"][var_str].get<bool>();
}

std::optional<unsigned int> Config::GetUintForCrashGuardVar(Config::Var var)
{
    std::string var_str;

    switch(var) {
    case Var::CrashGuard_OfflineImmunitySeconds:
        var_str = "OfflineImmunitySeconds";
        break;

    case Var::CrashGuard_ReconnectImmunitySeconds:
        var_str = "ReconnectImmunitySeconds";
        break;

    default:
        Log::GetLog()->error("{}:{}: invalid config var", __func__, __LINE__);
        return std::nullopt;
    }

    return Config::Impl["CrashGuard"][var_str].get<unsigned int>();
}

std::string Config::GetConfigPath()
{
    return ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/Bodyguard/config.json";
}

bool Config::ReadFromFile()
{
    const auto config_path = GetConfigPath();
    std::ifstream in_file(config_path);

    if(in_file.is_open() == false) {
        Log::GetLog()->error("{}:{}: failed to open '{}'. Using default config", __func__, __LINE__, config_path);
        Config::Impl = Config::Default;
        return false;
    }
    else {
        json config;

        try {
            in_file >> config;
        }
        catch(json::parse_error& e) {
            Log::GetLog()->error("{}:{}: Exception:{}: {}", __func__, __LINE__, e.id, e.what());
            return false;
        }

        if(IsValid(config)) {
            Config::Impl = config;
            return true;
        }
        else {
            Log::GetLog()->error("{}:{}: config failed validation. Using default config", __func__, __LINE__);
            Config::Impl = Config::Default;
            return false;
        }
    }
}

bool Config::WriteDefaultConfigToFile()
{
    const auto config_path = GetConfigPath();
    std::ofstream out_file(config_path);

    if(out_file.is_open() == true) {
        try {
            out_file << std::setw(4) << Config::Default;
            return true;
        }
        catch(json::type_error& e) {
            Log::GetLog()->error("{}:{}: Exception:{}: {}", __func__, __LINE__, e.id, e.what());
            return false;
        }
    }
    else {
        Log::GetLog()->error("{}:{}: failed to open '{}' for writing", __func__, __LINE__, config_path);
        return false;
    }
}

void Config::Cmd_ReloadConfig(APlayerController* _pc, FString* message, bool log)
{
    if(_pc == nullptr) {
        Log::GetLog()->error("{}:{}: APlayerController is null", __func__, __LINE__);
        return;
    }

    bool success = Config::ReadFromFile();
    const auto pc = static_cast<AShooterPlayerController*>(_pc);

    if(success == true) {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Config reloaded");
    }
    else {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Red, "Failed to reload config");
    }
}

void Config::Cmd_WriteDefaultConfigToFile(APlayerController* _pc, FString* message, bool log)
{
    if(_pc == nullptr) {
        Log::GetLog()->error("{}:{}: APlayerController is null", __func__, __LINE__);
        return;
    }

    bool success = Config::WriteDefaultConfigToFile();
    const auto pc = static_cast<AShooterPlayerController*>(_pc);

    if(success == true) {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Green, "Default config written to file");
    }
    else {
        ArkApi::GetApiUtils().SendServerMessage(pc, FColorList::Red, "Failed to write default config to file");
    }
}

bool Config::IsValid(const json& config)
{
    if(const auto admins_it = config.find("Admins"); admins_it != config.cend()) {
        if(admins_it->type() != json::value_t::object) {
            Log::GetLog()->error("{}:{}: 'Admins' value in json config isn't a valid object", __func__, __LINE__);
            return false;
        }

        if(const auto ddd_it = admins_it->find("DeadDinoDragging"); ddd_it != admins_it->cend()) {
            if(ddd_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'DeadDinoDragging' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadDinoDragging' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto ddi_it = admins_it->find("DeadDinoInventory"); ddi_it != admins_it->cend()) {
            if(ddi_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'DeadDinoInventory' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadDinoInventory' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto dpd_it = admins_it->find("DeadPlayerDragging"); dpd_it != admins_it->cend()) {
            if(dpd_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'DeadPlayerDragging' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadPlayerDragging' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto dpi_it = admins_it->find("DeadPlayerInventory"); dpi_it != admins_it->cend()) {
            if(dpi_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'DeadPlayerInventory' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadPlayerInventory' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto opd_it = admins_it->find("OfflinePlayerDragging"); opd_it != admins_it->cend()) {
            if(opd_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'OfflinePlayerDragging' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'OfflinePlayerDragging' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto opi_it = admins_it->find("OfflinePlayerInventory"); opi_it != admins_it->cend()) {
            if(opi_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'OfflinePlayerInventory' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'OfflinePlayerInventory' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto tpd_it = admins_it->find("TranqdPlayerDragging"); tpd_it != admins_it->cend()) {
            if(tpd_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'TranqdPlayerDragging' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'TranqdPlayerDragging' key in 'Admins' object", __func__, __LINE__);
            return false;
        }

        if(const auto tpi_it = admins_it->find("TranqdPlayerInventory"); tpi_it != admins_it->cend()) {
            if(tpi_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'TranqdPlayerInventory' value in 'Admins' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'TranqdPlayerInventory' key in 'Admins' object", __func__, __LINE__);
            return false;
        }
    }
    else {
        Log::GetLog()->error("{}:{}: failed to find 'Admins' key in json config", __func__, __LINE__);
        return false;
    }

    if(const auto crashguard_it = config.find("CrashGuard"); crashguard_it != config.cend()) {
        if(crashguard_it->type() != json::value_t::object) {
            Log::GetLog()->error("{}:{}: 'CrashGuard' value in json config isn't a valid object", __func__, __LINE__);
            return false;
        }

        if(const auto enable_it = crashguard_it->find("Enable"); enable_it != crashguard_it->cend()) {
            if(enable_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'Enable' value in 'CrashGuard' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'Enable' key in 'CrashGuard' object", __func__, __LINE__);
            return false;
        }

        if(const auto pi_it = crashguard_it->find("PermanentImmunity"); pi_it != crashguard_it->cend()) {
            if(pi_it->type() != json::value_t::boolean) {
                Log::GetLog()->error("{}:{}: 'PermanentImmunity' value in 'CrashGuard' object isn't a boolean", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'PermanentImmunity' key in 'CrashGuard' object", __func__, __LINE__);
            return false;
        }

        if(const auto ois_it = crashguard_it->find("OfflineImmunitySeconds"); ois_it != crashguard_it->cend()) {
            if(ois_it->type() != json::value_t::number_unsigned) {
                Log::GetLog()->error("{}:{}: 'OfflineImmunitySeconds' value in 'CrashGuard' object isn't an unsigned integer", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'OfflineImmunitySeconds' key in 'CrashGuard' object", __func__, __LINE__);
            return false;
        }

        if(const auto ris_it = crashguard_it->find("ReconnectImmunitySeconds"); ris_it != crashguard_it->cend()) {
            if(ris_it->type() != json::value_t::number_unsigned) {
                Log::GetLog()->error("{}:{}: 'ReconnectImmunitySeconds' value in 'CrashGuard' object isn't an unsigned integer", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'ReconnectImmunitySeconds' key in 'CrashGuard' object", __func__, __LINE__);
            return false;
        }
    }
    else {
        Log::GetLog()->error("{}:{}: failed to find 'CrashGuard' key in json config", __func__, __LINE__);
        return false;
    }

    if(const auto players_it = config.find("Players"); players_it != config.cend()) {
        if(players_it->type() != json::value_t::object) {
            Log::GetLog()->error("{}:{}: 'Players' value in json config isn't a valid object", __func__, __LINE__);
            return false;
        }

        if(const auto ddd_it = players_it->find("DeadDinoDragging"); ddd_it != players_it->cend()) {
            if(ddd_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'DeadDinoDragging' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadDinoDragging' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto ddi_it = players_it->find("DeadDinoInventory"); ddi_it != players_it->cend()) {
            if(ddi_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'DeadDinoInventory' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadDinoInventory' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto dpd_it = players_it->find("DeadPlayerDragging"); dpd_it != players_it->cend()) {
            if(dpd_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'DeadPlayerDragging' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadPlayerDragging' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto dpi_it = players_it->find("DeadPlayerInventory"); dpi_it != players_it->cend()) {
            if(dpi_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'DeadPlayerInventory' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'DeadPlayerInventory' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto opd_it = players_it->find("OfflinePlayerDragging"); opd_it != players_it->cend()) {
            if(opd_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'OfflinePlayerDragging' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'OfflinePlayerDragging' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto opi_it = players_it->find("OfflinePlayerInventory"); opi_it != players_it->cend()) {
            if(opi_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'OfflinePlayerInventory' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'OfflinePlayerInventory' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto tpd_it = players_it->find("TranqdPlayerDragging"); tpd_it != players_it->cend()) {
            if(tpd_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'TranqdPlayerDragging' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'TranqdPlayerDragging' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        if(const auto tpi_it = players_it->find("TranqdPlayerInventory"); tpi_it != players_it->cend()) {
            if(tpi_it->type() != json::value_t::string) {
                Log::GetLog()->error("{}:{}: 'TranqdPlayerInventory' value in 'Players' object isn't a valid string", __func__, __LINE__);
                return false;
            }
        }
        else {
            Log::GetLog()->error("{}:{}: failed to find 'TranqdPlayerInventory' key in 'Players' object", __func__, __LINE__);
            return false;
        }

        for(auto& e : players_it->items()) {
            const auto access_str = e.value().get<std::string>();
            const auto access_level_opt = GetAccessLevelFromString(access_str);

            if(access_level_opt.has_value() == false) {
                Log::GetLog()->error("{}:{}: failed to get access level for key '{}' with value '{}'", __func__, __LINE__, e.key(), access_str);
                return false;
            }
        }
    }
    else {
        Log::GetLog()->error("{}:{}: failed to find 'Players' key in json config", __func__, __LINE__);
        return false;
    }

    return true;
}