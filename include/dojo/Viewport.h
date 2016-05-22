/*
 *  Viewport.h
 *  NinjaTraining
 *
 *  Created by Tommaso Checchi on 4/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "dojo_common_header.h"

#include "Object.h"
#include "Plane.h"
#include "Vector.h"
#include "Color.h"
#include "Renderable.h"
#include "Platform.h"
#include "Radians.h"
#include "Framebuffer.h"

namespace Dojo {
	class Renderer;
	class GameState;
	class AnimatedQuad;
	class Renderable;
	class RenderLayer;
	class Texture;
	class RenderSurface;

	///A Viewport is a View in a Dojo GameState, working both in 2D and 3D
	/**
	when rendering a 2D layer, the Viewport uses an orthogonal transform and manages
	the pixel-perfect rendering with its targetSize property;
	when rendering a 3D layer, it culls the scene and renders the perspective using its Frustum
	*/
	//TODO a Viewport should be a component!
	class Viewport : public Component {
	public:
		static const int ID = ComponentID::Viewport;

		typedef std::vector<RenderLayer::ID> LayerList;

		Viewport(
			Object& parent,
			const Vector& worldSize2D,
			const Color& clear,
			Degrees VFOV = 0.0_deg,
			float zNear = 0.01f,
			float zFar = 1000.f,
			int renderingOrder = -1);

		virtual ~Viewport();

		///enable this viewport for frustum culling, setting the frustum values
		void enableFrustum(Degrees VFOV, float zNear, float zFar);

		///adds a Fader object (fullscreen colored quad) at the given level to the Viewport
		//void addFader(RenderLayer::ID layer);

		void setClearColor(const Color& color) {
			mClearColor = color;
		}

		///sets which subset of Render Layers this Viewport is able to "see"
		void setVisibleLayers(const LayerList& layers);

		///sets which subset of Render Layers this Viewport is able to "see"
		void addVisibleLayersRange(RenderLayer::ID min, RenderLayer::ID max);

		///returns the subset of visible layers that has been set by setVisibleLayers
		/**
		by default, the set is empty, which means "all layers"
		*/
		const LayerList& getVisibleLayers() const {
			return mLayerList;
		}
		
		const Color& getClearColor() const {
			return mClearColor;
		}

		Vector get2DExtents() const {
			return m2DRect * 0.5f;
		}

		const Vector& get2DRect() const {
			return m2DRect;
		}

		Degrees getVFOV() const {
			return mVFOV;
		}

		float getZFar() {
			return mZFar;
		}

		float getZNear() {
			return mZNear;
		}

		const Vector* getWorldFrustumVertices() {
			return mWorldFrustumVertices;
		}

		const Vector* getLocalFrustumVertices() {
			return mLocalFrustumVertices;
		}

		const Matrix& getViewTransform() {
			return mViewTransform;
		}

		const AABB& getGraphicsAABB() const;

		void setClearEnabled(bool enabled) {
			mEnableClear = enabled;
		}

		bool getClearEnabled() const {
			return mEnableClear;
		}

		Framebuffer& getFramebuffer() {
			return mFramebuffer;
		}

		///returns the on-screen position of the given world-space vector
		Vector getScreenPosition(const Vector& pos);

		///given a [0,1] normalized SS pos, returns the direction of the world space ray it originates
		Vector getRayDirection(const Vector& screenSpacePos);

		const Matrix& getOrthoProjectionTransform() {
			return mOrthoTransform;
		}

		const Matrix& getPerspectiveProjectionTransform() {
			_updateFrustum();
			return mFrustumTransform;
		}

		bool isContainedInFrustum(const Renderable& r) const;

		bool isVisible(Renderable& s);

		bool isInViewRect(const Renderable& r) const;
		bool isInViewRect(const AABB& pos) const;
		bool isInViewRect(const Vector& pos) const;

		///returns the world position of the given screenPoint
		Vector makeWorldCoordinates(const Vector& screenPoint) const;

		///returns the world position of the given screenPoint
		Vector makeWorldCoordinates(int x, int y) const;

		///converts the w and h pixel sizes in a screen space size
		Vector makeScreenSize(uint32_t w, uint32_t h) const;

		float getPixelSide() const;

		void setEyeTransform(const Matrix& t) {
			mPerspectiveEyeTransform = t;
		}

		void _update();

		void onAttach() override;
		void onDetach() override;

	protected:

		Vector m2DRect;

		bool mEnableClear = true, mFrustumDirty = true;

		Matrix mLastWorldTransform;

		Color mClearColor;

		Matrix mViewTransform, mOrthoTransform, mFrustumTransform, mPerspectiveEyeTransform;

		Vector mLocalFrustumVertices[4];
		Vector mWorldFrustumVertices[4];

		Plane mWorldFrustumPlanes[5];

		Degrees mVFOV;
		float mZNear, mZFar;
		Vector mFarPlaneSide;
		int mRenderingOrder = -1;

		LayerList mLayerList;
		Framebuffer mFramebuffer;

		AABB mWorldBB;

		void _updateFrustum();

		void _setRenderTarget(RenderSurface& surface);
	};
}
