use crate::app::config::AppConfiguration;
use crate::app::window::WindowState;
use hashbrown::HashMap;
use parking_lot::RwLock;
use std::sync::Arc;
use wgpu::{BackendOptions, Backends, InstanceDescriptor, InstanceFlags};
use winit::event_loop::ActiveEventLoop;
use winit::window::{Window, WindowId};

pub struct AppState {
    primary_window: RwLock<Option<Arc<WindowState>>>,
    secondary_windows: RwLock<HashMap<WindowId, Arc<WindowState>>>,
    instance: wgpu::Instance,
}

impl AppState {
    pub fn new(app_configuration: &AppConfiguration) -> Self {
        Self {
            primary_window: RwLock::default(),
            secondary_windows: RwLock::default(),
            instance: wgpu::Instance::new(&InstanceDescriptor {
                backends: Backends::all(),
                flags: InstanceFlags::default(),
                backend_options: BackendOptions::default(),
            }),
        }
    }

    // When using this, you might overwrite the previous primary window as there is no check. This should likely never be called except for the first call which actually sets the window (or if the window has to be recreated for some reason)
    pub fn register_primary_window(&self, window: Window) {
        *self.primary_window.write() = Some(Arc::new(self.create_window_state(window)));
    }

    pub fn register_secondary_window(&self, window: Window) {
        self.secondary_windows
            .write()
            .insert(window.id(), Arc::new(self.create_window_state(window)));
    }

    pub fn register_secondary_windows(&self, windows: Vec<Window>) {
        if windows.is_empty() {
            return;
        } // don't obtain a lock if we don't have to

        let mut write = self.secondary_windows.write();
        for window in windows {
            write.insert(window.id(), Arc::new(self.create_window_state(window)));
        }
    }

    // Try to avoid calling this often (store the arc instead)
    pub fn primary_window(&self) -> Option<Arc<WindowState>> {
        self.primary_window.read().clone()
    }

    pub fn secondary_windows(&self) -> Vec<Arc<WindowState>> {
        self.secondary_windows.read().values().cloned().collect()
    }

    pub fn window(&self, id: WindowId) -> Option<Arc<WindowState>> {
        if let Some(window) = self.primary_window.read().as_ref() {
            if window.id() == id {
                return Some(window.clone());
            }
        }

        self.secondary_window(id)
    }

    pub fn secondary_window(&self, id: WindowId) -> Option<Arc<WindowState>> {
        self.secondary_windows.read().get(&id).cloned()
    }

    pub fn on_window_close_request(&self, window_id: WindowId, event_loop: &ActiveEventLoop) {
        if let Some(pw) = self.primary_window.read().as_ref() {
            if pw.id() == window_id {
                event_loop.exit();
                return;
            }
        }

        _ = self.secondary_windows.write().remove(&window_id); // TODO: some kind of signal or event that a window is being closed here so that we can clean up necessary resources
    }

    fn create_window_state(&self, window: Window) -> WindowState {
        WindowState::new(&self.instance, window).expect("Failed to create window state")
    }
}
