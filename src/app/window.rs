use std::sync::Arc;
use thiserror::Error;
use wgpu::Surface;
use winit::window::{Window, WindowId};

pub struct WindowState {
    window: Arc<Window>,
    surface: Surface<'static>, // we pass the arc window into here
}

#[derive(Error, Debug)]
pub enum WindowStateCreateError {
    #[error(transparent)]
    CreateSurfaceError(#[from] wgpu::CreateSurfaceError),
}

impl WindowState {
    pub(super) fn new(
        instance: &wgpu::Instance,
        window: Window,
    ) -> Result<WindowState, WindowStateCreateError> {
        let window = Arc::new(window);
        let surface = instance.create_surface(window.clone())?;

        Ok(Self { window, surface })
    }
}

impl WindowState {
    pub fn id(&self) -> WindowId {
        self.window.id()
    }
    
    pub fn request_redraw(&self) {
        self.window.request_redraw();
    }
}
