#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Objects/Image.hpp>

#include <Tether/Module/Rendering/BufferedImage.hpp>

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedString.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT Renderer
	{
	public:
		Renderer();
		virtual ~Renderer();
		TETHER_NO_COPY(Renderer);

		void AddObject(Objects::Object* pObject);
		bool RemoveObject(Objects::Object* pObject);
		bool HasObject(Objects::Object* pObject);
		void ClearObjects();

		virtual bool RenderFrame() { return true; }

		template<typename T>
		Scope<T> CreateObject()
		{
			Scope<T> object;
			OnCreateObject(object);

			return object;
		}

		virtual Scope<BufferedImage> CreateImage(const BufferedImageInfo& info) = 0;

		const std::vector<Objects::Object*>& GetObjects() const;
	protected:
		virtual void OnObjectAdd(Objects::Object* pObject) {}
		virtual void OnObjectRemove(Objects::Object* pObject) {}
		
		virtual void OnCreateObject(Scope<Objects::Rectangle>& object) = 0;
		virtual void OnCreateObject(Scope<Objects::Image>& object) = 0;

		std::vector<Objects::Object*> objects;
	};
}
