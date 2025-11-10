#pragma once
#include "raylib_include.h"
#include "Event.h"

namespace core {

	// Base Layer class
	// Layers are used to organize different parts of the application
	// Each layer has its own update, render, and event handling
	class Layer {
	public:
		Layer(const std::string& name = "Layer")
			: m_debugName(name), m_initialized(false) {}

		virtual ~Layer() = default;

		// Non-virtual wrapper that handles initialization state
		// Calls OnInitialize() which derived classes should override
		void Initialize() {
			if (!m_initialized) {
				OnInitialize();
				m_initialized = true;
			}
		}

		// Non-virtual wrapper that handles cleanup state
		// Calls OnUninitialize() which derived classes should override
		void Uninitialize() {
			if (m_initialized) {
				OnUninitialize();
				m_initialized = false;
			}
		}

		// Called every frame for logic updates
		// deltaTime is the time in seconds since the last frame
		virtual void Update(float deltaTime) {}

		// Called every frame for rendering
		// This is called after Update
		virtual void Render() {}

		// Called when an event occurs
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_debugName; }
		inline bool IsInitialized() const { return m_initialized; }

	protected:
		// Override these in derived classes for custom initialization/cleanup
		// No need to call base class versions
		virtual void OnInitialize() {}
		virtual void OnUninitialize() {}

		std::string m_debugName;
		bool m_initialized;
	};

	// Layer Stack - manages a collection of layers
	class LayerStack {
	public:
		LayerStack() = default;
		~LayerStack();

		// Push a layer to the end of the stack
		void PushLayer(std::unique_ptr<Layer> layer);

		// Push an overlay to the very end (rendered last, receives events first)
		void PushOverlay(std::unique_ptr<Layer> overlay);

		// Pop a layer from the stack
		void PopLayer(Layer* layer);

		// Pop an overlay from the stack
		void PopOverlay(Layer* overlay);

		// Initialize all layers
		void InitializeAll();

		// Uninitialize all layers
		void UninitializeAll();

		// Update all layers
		void UpdateAll(float deltaTime);

		// Render all layers
		void RenderAll();

		// Propagate events through the layer stack (from top to bottom)
		void PropagateEvent(Event& event);

		// Get layer by name
		Layer* GetLayer(const std::string& name);

		// Iterators
		std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
		std::vector<std::unique_ptr<Layer>>::iterator end() { return m_layers.end(); }
		std::vector<std::unique_ptr<Layer>>::const_iterator begin() const { return m_layers.begin(); }
		std::vector<std::unique_ptr<Layer>>::const_iterator end() const { return m_layers.end(); }

		// Reverse iterators for event propagation (top to bottom)
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() { return m_layers.rbegin(); }
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() { return m_layers.rend(); }

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_layerInsertIndex = 0; // Separates layers from overlays
	};

} // namespace core
