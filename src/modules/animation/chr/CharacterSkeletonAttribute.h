/**
 * @file
 */

#pragma once

#include "animation/SkeletonAttribute.h"

namespace animation {

/**
 * @brief The skeleton attributes reflect the model values that are needed to
 * assemble the final mesh. This is mostly about offsets and positioning.
 */
struct CharacterSkeletonAttribute {
	float scaler = 1.0f;
	float toolRight = 6.0f;
	float toolForward = -6.1f;
	float toolScale = 1.0f;
	float neckRight = 0.0f;
	float neckForward = 0.0f;
	float neckHeight = 0.0f;
	float headScale = 1.0f;
	float handRight = -7.5f;
	float handForward = 0.0f;
	float shoulderRight = -5.0f;
	float shoulderForward = 0.0f;

	float runTimeFactor = 12.0f;
	float jumpTimeFactor = 14.0f;
	float idleTimeFactor = 0.3f;

	float shoulderScale = 1.1f;
	float hipOffset = 6.0f; // to shift the rotation point for the feet
	float origin = 0.0f;
	float footHeight = 3.0f;
	float invisibleLegHeight = 0.5f;
	float pantsHeight = 3.0f;
	float beltHeight = 2.0f;
	float chestHeight = 5.0f;
	float headHeight = 9.0f;
	float footRight = -3.2f;

	// not exposed but calculated
	float footY;
	float pantsY;
	float beltY;
	float chestY;
	float headY;
	float gliderY;

	/**
	 * @brief Updates some absolute values that depend on other scriptable values
	 * @note Make sure to call init() after you modified the values
	 */
	bool init() {
		footY = origin;
		pantsY = footY + footHeight + invisibleLegHeight;
		beltY = pantsY + pantsHeight;
		chestY = beltY + beltHeight;
		headY = chestY + chestHeight;
		gliderY = headY + headHeight;
		// TODO: perform sanity checks
		return true;
	}
};

static const SkeletonAttributeMeta ChrSkeletonAttributeMetaArray[] = {
	{ "scaler",             offsetof(struct CharacterSkeletonAttribute, scaler) },
	{ "toolRight",          offsetof(struct CharacterSkeletonAttribute, toolRight) },
	{ "toolForward",        offsetof(struct CharacterSkeletonAttribute, toolForward) },
	{ "toolScale",          offsetof(struct CharacterSkeletonAttribute, toolScale) },
	{ "neckRight",          offsetof(struct CharacterSkeletonAttribute, neckRight) },
	{ "neckForward",        offsetof(struct CharacterSkeletonAttribute, neckForward) },
	{ "neckHeight",         offsetof(struct CharacterSkeletonAttribute, neckHeight) },
	{ "headScale",          offsetof(struct CharacterSkeletonAttribute, headScale) },
	{ "handRight",          offsetof(struct CharacterSkeletonAttribute, handRight) },
	{ "handForward",        offsetof(struct CharacterSkeletonAttribute, handForward) },
	{ "shoulderRight",      offsetof(struct CharacterSkeletonAttribute, shoulderRight) },
	{ "shoulderForward",    offsetof(struct CharacterSkeletonAttribute, shoulderForward) },
	{ "runTimeFactor",      offsetof(struct CharacterSkeletonAttribute, runTimeFactor) },
	{ "jumpTimeFactor",     offsetof(struct CharacterSkeletonAttribute, jumpTimeFactor) },
	{ "idleTimeFactor",     offsetof(struct CharacterSkeletonAttribute, idleTimeFactor) },
	{ "shoulderScale",      offsetof(struct CharacterSkeletonAttribute, shoulderScale) },
	{ "hipOffset",          offsetof(struct CharacterSkeletonAttribute, hipOffset) },
	{ "origin",             offsetof(struct CharacterSkeletonAttribute, origin) },
	{ "footHeight",         offsetof(struct CharacterSkeletonAttribute, footHeight) },
	{ "invisibleLegHeight", offsetof(struct CharacterSkeletonAttribute, invisibleLegHeight) },
	{ "pantsHeight",        offsetof(struct CharacterSkeletonAttribute, pantsHeight) },
	{ "beltHeight",         offsetof(struct CharacterSkeletonAttribute, beltHeight) },
	{ "chestHeight",        offsetof(struct CharacterSkeletonAttribute, chestHeight) },
	{ "headHeight",         offsetof(struct CharacterSkeletonAttribute, headHeight) },
	{ "footRight",          offsetof(struct CharacterSkeletonAttribute, footRight) },
	{ nullptr, 0u }
};

}