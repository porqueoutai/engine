/**
 * @file
 */

#pragma once

#include "video/Camera.h"
#include "voxelworld/WorldMgr.h"
#include "WorldRenderer.h"
#include "core/command/ActionButton.h"

namespace voxelrender {

/**
 * @brief The player camera clips against the world while moving.
 */
class PlayerCamera {
private:
	video::Camera _camera;
	voxelworld::WorldMgrPtr _worldMgr;
	WorldRenderer& _worldRenderer;

	core::VarPtr _maxTargetDistance;
	core::VarPtr _cameraZoomSpeed;

	core::ActionButton _zoomIn;
	core::ActionButton _zoomOut;

	float _fieldOfView = 60.0f;
	float _targetDistance = 28.0f;
	glm::vec3 _cameraPosition {1.0f, 0.4f, 1.0f};
	float _pendingPitch = 0.0f;
	float _pendingTurn = 0.0f;
	float _pendingSpeed = -1.0f;

	void zoom(float level);

public:
	PlayerCamera(const voxelworld::WorldMgrPtr &world, voxelrender::WorldRenderer &worldRenderer) :
			_worldMgr(world), _worldRenderer(worldRenderer) {
	}

	void setTarget(const glm::vec3& position);
	void setTargetDistance(float targetDistance);

	void construct();
	bool init(const glm::ivec2& position, const glm::ivec2& frameBufferSize, const glm::ivec2& windowSize);
	void shutdown();
	void update(const glm::vec3& entityPosition, int64_t deltaFrame, uint64_t now);

	void setFieldOfView(float fieldOfView);
	void rotate(float pitch, float turn, float speed);

	const video::Camera& camera() const;
};

inline void PlayerCamera::setTargetDistance(float targetDistance) {
	_targetDistance = targetDistance;
}

inline void PlayerCamera::setTarget(const glm::vec3& position) {
	_camera.setTarget(position);
}

inline void PlayerCamera::setFieldOfView(float fieldOfView) {
	_camera.setFieldOfView(fieldOfView);
}

inline const video::Camera& PlayerCamera::camera() const {
	return _camera;
}

}
