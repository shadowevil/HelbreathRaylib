#include "Layer.h"

namespace core {

	LayerStack::~LayerStack() {
		UninitializeAll();
		m_layers.clear();
	}

	void LayerStack::PushLayer(std::unique_ptr<Layer> layer) {
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, std::move(layer));
		m_layerInsertIndex++;
	}

	void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay) {
		m_layers.emplace_back(std::move(overlay));
	}

	void LayerStack::PopLayer(Layer* layer) {
		auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layerInsertIndex,
			[layer](const std::unique_ptr<Layer>& l) { return l.get() == layer; });

		if (it != m_layers.begin() + m_layerInsertIndex) {
			(*it)->Uninitialize();
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay) {
		auto it = std::find_if(m_layers.begin() + m_layerInsertIndex, m_layers.end(),
			[overlay](const std::unique_ptr<Layer>& l) { return l.get() == overlay; });

		if (it != m_layers.end()) {
			(*it)->Uninitialize();
			m_layers.erase(it);
		}
	}

	void LayerStack::InitializeAll() {
		for (auto& layer : m_layers) {
			if (!layer->IsInitialized()) {
				layer->Initialize();
			}
		}
	}

	void LayerStack::UninitializeAll() {
		for (auto& layer : m_layers) {
			if (layer->IsInitialized()) {
				layer->Uninitialize();
			}
		}
	}

	void LayerStack::UpdateAll(float deltaTime) {
		for (auto& layer : m_layers) {
			if (layer->IsInitialized()) {
				layer->Update(deltaTime);
			}
		}
	}

	void LayerStack::RenderAll() {
		for (auto& layer : m_layers) {
			if (layer->IsInitialized()) {
				layer->Render();
			}
		}
	}

	void LayerStack::PropagateEvent(Event& event) {
		// Events propagate from top to bottom (reverse order)
		// Overlays receive events first, then layers
		for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
			if (event.handled)
				break;

			if ((*it)->IsInitialized()) {
				(*it)->OnEvent(event);
			}
		}
	}

	Layer* LayerStack::GetLayer(const std::string& name) {
		for (auto& layer : m_layers) {
			if (layer->GetName() == name) {
				return layer.get();
			}
		}
		return nullptr;
	}

} // namespace core
