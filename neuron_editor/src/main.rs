mod project;

use neuron_engine::app::config::{AppConfiguration, WindowConfiguration};
use neuron_engine::app::{App, AppError};

fn main() -> Result<(), AppError> {
    env_logger::init();

    App::new(AppConfiguration {
        extra_windows: vec![WindowConfiguration {
            title: "Test Secondary".to_string(),
            ..Default::default()
        }],
        ..Default::default()
    })?.run()
}
