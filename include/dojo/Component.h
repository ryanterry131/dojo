#pragma once

#include "dojo_common_header.h"

namespace Dojo {
	class Object;

	class Component {
	public:
		Object& object;

		Component(Object& object) :
			object(object) {

		}

		Object& getObject() {
			return object;
		}

		const Object& getObject() const {
			return object;
		}

		virtual void onAttach() {}
		virtual void onDetach() {}

		virtual void onDispose() {}

		///"authorization" method useful to ask the engine to keep the objects alive for longer after disposed of
		virtual bool canDestroy() const {
			return true;
		}
	};
}
