mod project;

use thiserror::Error;
use crate::project::buildsystem::BuildsystemConfiguration;
use crate::project::manifest::ProjectManifest;
use neuron_engine::app::config::{AppConfiguration, WindowConfiguration};
use neuron_engine::app::{App, AppError};
use crate::project::generate::{generate_project_skeleton, GenerateProjectError};

#[derive(Error, Debug)]
enum EditorError {
    #[error(transparent)]
    AppError(#[from] AppError),

    #[error(transparent)]
    GenerateProjectError(#[from] GenerateProjectError),
}

fn main() -> Result<(), EditorError> {
    env_logger::init();

    // test buildsystem
    let manifest = ProjectManifest {
        name: "Test Project".to_string(),
        build: BuildsystemConfiguration {
            name: "test-project".to_string(),
            ..Default::default()
        },
    };
    generate_project_skeleton("test_project_dir", manifest)?;

    App::new(AppConfiguration {
        extra_windows: vec![WindowConfiguration {
            title: "Test Secondary".to_string(),
            ..Default::default()
        }],
        ..Default::default()
    })?
    .run().map_err(|e| EditorError::AppError(e))
}
