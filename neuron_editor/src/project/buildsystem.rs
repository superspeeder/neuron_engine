use std::io;
use std::path::Path;

#[derive(Clone, serde::Deserialize, serde::Serialize)]
pub struct BuildsystemConfiguration {
    pub name: String,
    pub api_version_tag: String,
}

impl Default for BuildsystemConfiguration {
    fn default() -> Self {
        Self {
            name: "Project".to_string(),
            api_version_tag: "master".to_string(), // by default, we are just going to use the master branch. In the future, we will have a tag per crate version and use the one that the editor is built for instead
        }
    }
}

const NEURON_SCRIPT_API_GIT_REPO: &'static str = "https://github.com/superspeeder/neuron_engine.git";

pub fn generate_cargo_toml(buildsystem_configuration: &BuildsystemConfiguration) -> String {
    format!(include_str!("Cargo-template.toml"), name=buildsystem_configuration.name, api_git_repo=NEURON_SCRIPT_API_GIT_REPO, api_git_tag=buildsystem_configuration.api_version_tag)
}

pub fn generate_build_project(project_root: impl AsRef<Path>, buildsystem_configuration: &BuildsystemConfiguration) -> Result<(), io::Error> {
    let build_dir = project_root.as_ref().join(".build");
    std::fs::create_dir_all(&build_dir)?;
    std::fs::write(build_dir.join("Cargo.toml"), generate_cargo_toml(buildsystem_configuration))?;
    
    Ok(())
}