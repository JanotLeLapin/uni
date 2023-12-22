mod parser;
mod util;

const OUT: &str = "dist";

fn compile_markdown(name: &str, _ext: &str, path: &std::path::Path) {
    if let Ok(content) = std::fs::read_to_string(path) {
        let html = parser::parse_markdown(&content);
        let page = parser::make_page(html);

        let parent = format!("{}/{}", OUT, path.parent().unwrap().display());
        let _ = std::fs::create_dir_all(&parent);
        let _ = std::fs::write(format!("{}/{}", &parent, format!("{}.html", name)), page);
    }
}

fn main() {
    let _ = std::fs::create_dir_all(format!("{}/content", OUT));
    let _ = std::fs::create_dir_all(format!("{}/style", OUT));

    util::walk_dir("content", compile_markdown);
}
