pub mod config;
mod state;
mod window;

use crate::app::config::AppConfiguration;
use crate::app::state::AppState;
use std::process::ExitCode;
use std::sync::Arc;
use std::time::Duration;
use thiserror::Error;
use winit::application::ApplicationHandler;
use winit::error::EventLoopError;
use winit::event::WindowEvent;
use winit::event_loop::{ActiveEventLoop, EventLoop};
use winit::platform::pump_events::{EventLoopExtPumpEvents, PumpStatus};
use winit::window::WindowId;

pub struct App {
    event_loop: Option<EventLoop<()>>,
    configuration: AppConfiguration,
    state: Arc<AppState>,
}

#[derive(Debug, Error)]
pub enum AppError {
    #[error(transparent)]
    EventLoopError(#[from] EventLoopError),

    #[error("No event loop.")]
    NoEventLoop,

    #[error("Exit with code: {0:?}")]
    ExitWithCode(i32),
}

impl App {
    pub fn new(configuration: AppConfiguration) -> Result<Self, AppError> {
        let state = Arc::new(AppState::new(&configuration));

        Ok(Self {
            event_loop: Some(EventLoop::new()?),
            configuration,
            state,
        })
    }

    pub fn run(mut self) -> Result<(), AppError> {
        let Some(mut event_loop) = self.event_loop.take() else {
            return Err(AppError::NoEventLoop);
        };

        'mainloop: loop {
            let status = event_loop.pump_app_events(Some(Duration::ZERO), &mut self);

            match status {
                PumpStatus::Continue => {}
                PumpStatus::Exit(code) => {
                    if code != 0 {
                        return Err(AppError::ExitWithCode(code)); // non-zero exit indicates crash
                    } else {
                        break 'mainloop;
                    }
                }
            }
        }

        Ok(())
    }

    async fn redraw(&mut self, window_id: WindowId) {}
}

impl ApplicationHandler for App {
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        self.state.register_primary_window(
            event_loop
                .create_window(self.configuration.primary_window.create_window_attributes())
                .unwrap(),
        );

        let secondary_windows = self
            .configuration
            .extra_windows
            .iter()
            .map(|c| {
                event_loop
                    .create_window(c.create_window_attributes())
                    .unwrap()
            })
            .collect::<Vec<_>>();
        self.state.register_secondary_windows(secondary_windows);
    }

    fn window_event(
        &mut self,
        event_loop: &ActiveEventLoop,
        window_id: WindowId,
        event: WindowEvent,
    ) {
        match event {
            WindowEvent::RedrawRequested => pollster::block_on(self.redraw(window_id)),
            WindowEvent::CloseRequested => {
                self.state.on_window_close_request(window_id, event_loop)
            }
            _ => (),
        }
    }

    fn about_to_wait(&mut self, event_loop: &ActiveEventLoop) {
        if let Some(window) = self.state.primary_window() {
            window.request_redraw();
        }

        for window in self.state.secondary_windows() {
            window.request_redraw();
        }
    }
}
