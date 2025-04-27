use winit::dpi::PhysicalPosition;
pub use winit::dpi::PhysicalSize;
use winit::window::WindowAttributes;

pub struct AppConfiguration {
    pub primary_window: WindowConfiguration,
    pub extra_windows: Vec<WindowConfiguration>,
}

pub struct WindowConfiguration {
    pub title: String,
    pub size: Option<PhysicalSize<u32>>,
    pub position: Option<PhysicalPosition<i32>>,
    pub resizable: bool,
}

impl Default for AppConfiguration {
    fn default() -> AppConfiguration {
        AppConfiguration {
            primary_window: Default::default(),
            extra_windows: Default::default(),
        }
    }
}

impl Default for WindowConfiguration {
    fn default() -> WindowConfiguration {
        WindowConfiguration {
            title: "Window".to_string(),
            size: None,
            position: None,
            resizable: false,
        }
    }
}

impl WindowConfiguration {
    pub(super) fn create_window_attributes(&self) -> WindowAttributes {
        let mut attr = WindowAttributes::default()
            .with_title(self.title.clone())
            .with_resizable(self.resizable);

        if let Some(size) = self.size {
            attr = attr.with_inner_size(size);
        }

        if let Some(position) = self.position {
            attr = attr.with_position(position);
        }

        attr
    }
}
