#include "TestOctreeVisit.h"

#include "io/Filesystem.h"
#include "imgui/IMGUI.h"
#include "core/Color.h"
#include "video/ScopedLineWidth.h"
#include <array>

TestOctreeVisit::TestOctreeVisit(const io::FilesystemPtr& filesystem, const core::EventBusPtr& eventBus, const core::TimeProviderPtr& timeProvider) :
		Super(filesystem, eventBus, timeProvider) {
	init(ORGANISATION, "testoctreevisit");
	setCameraMotion(true);
	//setRenderPlane(true);
	setRenderAxis(true);
	setCameraSpeed(0.5f);
}

core::AppState TestOctreeVisit::onInit() {
	core::AppState state = Super::onInit();

	_camera.setFarPlane(5000.0f);

	if (!_shapeRenderer.init()) {
		Log::error("Failed to init the shape renderer");
		return core::AppState::Cleanup;
	}
	return state;
}

void TestOctreeVisit::updateCamera() {
	_octreeCamera.init(glm::ivec2(), _ortho ? glm::ivec2(100, 50) : dimension());
	_octreeCamera.setOmega(_omega);

	_octreeCamera.setPosition(_pos);
	_octreeCamera.lookAt(_lookAt);
	_octreeCamera.setFarPlane(_farPlane);
	_octreeCamera.setNearPlane(_nearPlane);
	_octreeCamera.setRotationType(video::CameraRotationType::Target);
	if (_ortho) {
		_octreeCamera.setMode(video::CameraMode::Orthogonal);
	} else {
		_octreeCamera.setMode(video::CameraMode::Perspective);
	}
}

core::AppState TestOctreeVisit::onRunning() {
	core::AppState state = Super::onRunning();
	if (state != core::AppState::Running) {
		return state;
	}

	_octreeCamera.update(_deltaFrame);

	_shapeBuilder.clear();
	const int minSize = 64;
	_octree.visit(_octreeCamera.frustum(), [this] (const glm::vec3& center) {
		_shapeBuilder.setPosition(center);
		_shapeBuilder.sphere(10, 10, 5.0f);
		return true;
	}, glm::ivec3(minSize));
	_shapeRenderer.createOrUpdate(_itemMeshes, _shapeBuilder);

	_shapeBuilder.clear();
	_shapeBuilder.frustum(_octreeCamera);
	_shapeRenderer.createOrUpdate(_frustumMesh, _shapeBuilder);

	_shapeBuilder.clear();
	const core::AABB<int>& aabb = core::computeAABB(_octreeCamera.frustum(), glm::vec3(minSize));
	_shapeBuilder.aabb(aabb);
	_shapeRenderer.createOrUpdate(_aabbMesh, _shapeBuilder);

	return state;
}

void TestOctreeVisit::onRenderUI() {
	ImGui::SetNextWindowSize(ImVec2(400, 220));
	ImGui::Begin("Options");
	ImGui::InputFloat3("position", glm::value_ptr(_pos));
	ImGui::InputFloat3("lookat", glm::value_ptr(_lookAt));
	ImGui::InputFloat3("omega", glm::value_ptr(_omega));
	ImGui::InputFloat("farplane", &_farPlane);
	ImGui::InputFloat("nearplane", &_nearPlane);
	ImGui::Checkbox("orthogonal", &_ortho);
	if (ImGui::Button("update")) {
		updateCamera();
	}
	ImGui::Separator();
	Super::onRenderUI();
	ImGui::End();
}

void TestOctreeVisit::doRender() {
	if (_itemMeshes != -1) {
		_shapeRenderer.render(_itemMeshes, _camera);
	}
	if (_aabbMesh != -1) {
		_shapeRenderer.render(_aabbMesh, _camera);
	}
	if (_frustumMesh != -1) {
		_shapeRenderer.render(_frustumMesh, _camera);
	}
}

core::AppState TestOctreeVisit::onCleanup() {
	core::AppState state = Super::onCleanup();
	_shapeRenderer.shutdown();
	return state;
}

int main(int argc, char *argv[]) {
	const core::EventBusPtr eventBus = std::make_shared<core::EventBus>();
	const io::FilesystemPtr filesystem = std::make_shared<io::Filesystem>();
	const core::TimeProviderPtr timeProvider = std::make_shared<core::TimeProvider>();
	TestOctreeVisit app(filesystem, eventBus, timeProvider);
	return app.startMainLoop(argc, argv);
}
