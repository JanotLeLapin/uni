use std::{
    ffi::{CStr, CString},
    os::raw::c_char,
};

use tree_sitter_highlight::{HighlightConfiguration, HighlightEvent, Highlighter};

extern "C" {
    fn tree_sitter_python() -> tree_sitter::Language;
}

#[no_mangle]
pub extern "C" fn uni_highlight(raw_source: *const c_char, raw_lang: *const c_char) -> *mut c_char {
    let highlight_names = [
        "attribute",
        "constant",
        "function.builtin",
        "function",
        "keyword",
        "operator",
        "property",
        "punctuation",
        "punctuation.bracket",
        "punctuation.delimiter",
        "string",
        "string.special",
        "tag",
        "type",
        "type.builtin",
        "variable",
        "variable.builtin",
        "variable.parameter",
        "comment",
        "number",
    ];

    let attribute_names = [
        "attr",
        "const",
        "fn builtin",
        "fn",
        "kw",
        "op",
        "prop",
        "punct",
        "punct bracket",
        "punct delim",
        "str",
        "str special",
        "tag",
        "t",
        "t builtin",
        "var",
        "var builtin",
        "var param",
        "comment",
        "num",
    ];

    if raw_source.is_null() || raw_lang.is_null() {
        return CString::new("").unwrap().into_raw();
    }

    let cstr_source = unsafe { CStr::from_ptr(raw_source) };
    let source = cstr_source.to_str().unwrap();

    let cstr_lang = unsafe { CStr::from_ptr(raw_lang) };
    let mut config = match cstr_lang.to_str().unwrap() {
        "py" | "python" => unsafe {
            HighlightConfiguration::new(
                tree_sitter_python(),
                "python",
                tree_sitter_python::HIGHLIGHTS_QUERY,
                "",
                "",
            )
        },
        _ => {
            println!("{source}");
            return CString::new("").unwrap().into_raw();
        }
    }
    .unwrap();

    let mut highlighter = Highlighter::new();
    config.configure(&highlight_names);

    let highlights = highlighter
        .highlight(&config, source.as_bytes(), None, |_| None)
        .unwrap();

    let mut buf = String::with_capacity(source.len() * 2);

    for event in highlights {
        match event.unwrap() {
            HighlightEvent::Source { start, end } => buf.push_str(&source[start..end]),
            HighlightEvent::HighlightStart(s) => {
                buf.push_str("<span class=\"");
                buf.push_str(attribute_names[s.0]);
                buf.push_str("\">");
            }
            HighlightEvent::HighlightEnd => buf.push_str("</span>"),
        }
    }

    return CString::new(buf).unwrap().into_raw();
}

#[no_mangle]
pub extern "C" fn uni_free_buffer(ptr: *mut c_char) {
    if ptr.is_null() {
        return;
    }

    unsafe {
        let _ = std::ffi::CString::from_raw(ptr);
    }
}
