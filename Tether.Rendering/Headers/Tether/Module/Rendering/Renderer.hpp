#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Objects/Text.hpp>

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Resources/Font.hpp>

#include <Tether/Common/TypeTools.hpp>
#include <Tether/Common/HashedType.hpp>
#include <Tether/Common/Types.hpp>

#include <Tether/Math/Types.hpp>

#include <type_traits>
#include <vector>

namespace Tether::Rendering
{
	class TETHER_EXPORT Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer();
		TETHER_NO_COPY(Renderer);

		void AddObject(Objects::Object& object);
		bool RemoveObject(Objects::Object& object);
		bool HasObject(Objects::Object& object);
		void ClearObjects();

		template<typename T, typename... Args>
			requires std::is_base_of_v<Objects::Object, T>
		Scope<T> CreateObject(Args... args)
		{
			Scope<T> object;
			OnCreateObject(object, args...);

			object->m_pRenderer = this;

			return object;
		}

		template<typename T>
			requires std::is_base_of_v<Objects::Object, T>
		Scope<T> CreateOtherObject()
		{
			Scope<T> object;
			OnCreateOtherObject(HashedType<T>(), object);

			object->m_pRenderer = this;

			return object;
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<Resources::Resource, T>
		Scope<T> CreateResource(Args... args)
		{
			Scope<T> resource;
			OnCreateResource(resource, args...);

			resource->m_pRenderer = this;

			return resource;
		}

		const std::vector<Objects::Object*>& GetObjects() const;
	protected:
		virtual void OnCreateObject(Scope<Objects::Rectangle>& object) = 0;
		virtual void OnCreateObject(Scope<Objects::Image>& object) = 0;
		virtual void OnCreateObject(Scope<Objects::Text>& object) = 0;

		virtual void OnCreateOtherObject(const HashedString& type,
			Scope<Objects::Object>& object) {}

		virtual void OnCreateResource(Scope<Resources::BufferedImage>& image, 
			const Resources::BufferedImageInfo& info) = 0;
		virtual void OnCreateResource(Scope<Resources::Font>& font,
			const std::string& fontPath) = 0;

		std::vector<Objects::Object*> objects;
	};
}
