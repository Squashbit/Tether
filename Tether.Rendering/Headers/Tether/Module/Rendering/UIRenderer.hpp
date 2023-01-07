#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedString.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT UIRenderer
	{
	public:
		UIRenderer();
		virtual ~UIRenderer();
		TETHER_NO_COPY(UIRenderer);

		void AddObject(Objects::Object* pObject);
		bool RemoveObject(Objects::Object* pObject);
		bool HasObject(Objects::Object* pObject);
		void ClearObjects();

		virtual bool RenderFrame() { return true; }

		template<typename T>
		Scope<T> CreateObject()
		{
			HashedString typeHash(TypeTools::GetTypeName<T>());
			return Scope<T>((T*)OnObjectCreate(typeHash));
		}

		const std::vector<Objects::Object*>& GetObjects() const;
	protected:
		virtual void OnObjectAdd(Objects::Object* pObject) {}
		virtual void OnObjectRemove(Objects::Object* pObject) {}

		// This function should return a heap allocated pointer.
		// It will get converted to a unique_ptr later, so no memory should be leaked.
		// The pointer returned should also be a class derived from Object.
		virtual Objects::Object* OnObjectCreate(HashedString& typeName) = 0;

		std::vector<Objects::Object*> objects;
	};
}
