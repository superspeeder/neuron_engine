use std::io;
use std::path::Path;

#[derive(Clone, serde::Deserialize, serde::Serialize)]
pub struct BuildsystemConfiguration {
    pub name: String,
    pub api_version_tag: Option<String>,
}

impl Default for BuildsystemConfiguration {
    fn default() -> Self {
        Self {
            name: "Project".to_string(),
            api_version_tag: None,
        }
    }
}

const NEURON_SCRIPT_API_GIT_REPO: &'static str = "https://github.com/superspeeder/neuron_engine.git";

pub fn generate_cargo_toml(buildsystem_configuration: &BuildsystemConfiguration) -> String {
    let versioning = buildsystem_configuration.api_version_tag.clone().map_or("".to_string(), |tag| format!(", tag=\"{tag}\""));
    format!(include_str!("../../template/Cargo-template.toml"), name=buildsystem_configuration.name, api_git_repo=NEURON_SCRIPT_API_GIT_REPO, api_git_versioning=versioning)
}

pub fn generate_build_project(project_root: impl AsRef<Path>, buildsystem_configuration: &BuildsystemConfiguration) -> Result<(), io::Error> {
    let build_dir = project_root.as_ref().join(".build");
    std::fs::create_dir_all(&build_dir)?;
    std::fs::write(project_root.as_ref().join("Cargo.toml"), generate_cargo_toml(buildsystem_configuration))?;
    
    Ok(())
}