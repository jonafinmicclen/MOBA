#pragma once

#include "Common/Player/AccountID.hpp"
#include "Game/Entities/Characters/CharacterID.hpp"

#include <unordered_map>
#include <optional>

class AccountCharacterBiMap {
public:
    AccountCharacterBiMap() = default;
    // Bind account <-> character (1:1)
    void bind(const AccountID& account, const CharacterID& character)
    {
        // If account was already bound, remove old reverse entry
        if (auto it = character_by_account_.find(account); it != character_by_account_.end()) {
            account_by_character_.erase(it->second);
        }

        // If character was already bound, remove old forward entry
        if (auto it = account_by_character_.find(character); it != account_by_character_.end()) {
            character_by_account_.erase(it->second);
        }

        character_by_account_[account] = character;
        account_by_character_[character] = account;
    }

    std::optional<CharacterID> characterFor(const AccountID& account) const
    {
        if (auto it = character_by_account_.find(account); it != character_by_account_.end())
            return it->second;

        return std::nullopt;
    }

    std::optional<AccountID> accountFor(const CharacterID& character) const
    {
        if (auto it = account_by_character_.find(character); it != account_by_character_.end())
            return it->second;

        return std::nullopt;
    }

    void unbindAccount(const AccountID& account)
    {
        if (auto it = character_by_account_.find(account); it != character_by_account_.end()) {
            account_by_character_.erase(it->second);
            character_by_account_.erase(it);
        }
    }

    void unbindCharacter(const CharacterID& character)
    {
        if (auto it = account_by_character_.find(character); it != account_by_character_.end()) {
            character_by_account_.erase(it->second);
            account_by_character_.erase(it);
        }
    }

    void clear()
    {
        character_by_account_.clear();
        account_by_character_.clear();
    }

private:
    std::unordered_map<AccountID, CharacterID> character_by_account_;
    std::unordered_map<CharacterID, AccountID> account_by_character_;
};