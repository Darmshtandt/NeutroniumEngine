#pragma once

#include <Objects/ObjectRegistrar.h>

class GameModel : public Entity {
public:
	GameModel(const std::string& name);
	~GameModel() noexcept override = default;

	[[nodiscard]] GameModel* GetCopy() const override;
	[[nodiscard]] static std::string GetClassToken() noexcept;
	[[nodiscard]] std::string GetToken() const noexcept override;
};