#pragma once

#include "Component.h"

#include "Vector.h"

namespace Dojo {
	class SoundListener : public Component {
	public:
		static const int ID = ComponentID::SoundListener;

		SoundListener(Object& parent, float zOffsetFor2D = 0);

		Matrix getTransform() const;

		virtual void onAttach() override;
		virtual void onDetach() override;

	private:
		float mZOffset;
	};
}

