#pragma once

#include "DraftLayer.h"
#include "DraftObject.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/signals2.hpp>
#include <spdlog/spdlog.h>
#include <list>

namespace CityDraft
{
	using LayerVisibilityChangedFunc = void(DraftLayer*);
	using LayerLockStateChangedFunc = void(DraftLayer*);
	using LayerNameChangedFunc = void(DraftLayer*);
	using LayerMarkedForDeletionFunc = void(DraftLayer*);

	using DraftObjectAddedFunc = void(DraftObject*);
	using DraftObjectRemovedFunc = void(DraftObject*);

	/// <summary>
	/// Container for all objects on the Draft
	/// </summary>
	class DraftScene
	{
	public:
		inline DraftScene(std::shared_ptr<spdlog::logger> logger) : m_Logger(logger) {};

		std::shared_ptr<DraftObject> AddDraftObject(DraftObject* obj);
		void RemoveDraftObject(DraftObject* objPtr);

		inline const std::vector<DraftLayer*>& GetLayers() const
		{
			std::vector<DraftLayer*> result;
			std::transform(m_Layers.begin(), m_Layers.end(), result.begin(), [](const auto& layerPtr) {return layerPtr.get(); });
			return result;
		}

	private:
		std::shared_ptr<spdlog::logger> m_Logger;

		std::list<std::shared_ptr<DraftLayer>> m_Layers;
		std::list<std::shared_ptr<DraftObject>> m_Objects;

		boost::signals2::signal<LayerVisibilityChangedFunc> m_LayerVisibilityChanged;
		boost::signals2::signal<LayerLockStateChangedFunc> m_LayerLockStateChanged;
		boost::signals2::signal<LayerNameChangedFunc> m_LayerNameChanged;
		boost::signals2::signal<LayerMarkedForDeletionFunc> m_LayerMarkedForDeletion;

		boost::signals2::signal<DraftObjectAddedFunc> m_DraftObjectAdded;
		boost::signals2::signal<DraftObjectRemovedFunc> m_DraftObjectRemoved;

		friend class DraftObject;
	};
}
