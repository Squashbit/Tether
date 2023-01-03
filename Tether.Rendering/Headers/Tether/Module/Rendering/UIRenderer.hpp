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
		Scope<Objects::ObjectRenderer> CreateObjectRenderer(T* pObject)
		{
			HashedString typeHash(TypeTools::GetTypeName<T>());
			return OnObjectCreateRenderer(typeHash, pObject);
		}

		const std::vector<Objects::Object*>& GetObjects() const;
	protected:
		virtual void OnObjectAdd(Objects::Object* pObject) {}
		virtual void OnObjectRemove(Objects::Object* pObject) {}
		virtual Scope<Objects::ObjectRenderer> OnObjectCreateRenderer(
			HashedString& typeName,
			Objects::Object* pObject
		) = 0;

		std::vector<Objects::Object*> objects;
	};
}
