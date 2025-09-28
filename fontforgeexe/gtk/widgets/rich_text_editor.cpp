/* Copyright 2024 Maxim Iorsh <iorsh@users.sourceforge.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "rich_text_editor.hpp"

#include <iostream>
#include <cstring>

#include "intl.h"
#include "../utils.hpp"

namespace ff::widget {

void dump_character(Glib::ustring& unicode_buffer, const gunichar& character) {
    Glib::ustring seq;
    switch (character) {
        case '<':
            seq = "&lt;";
            break;
        case '\"':
            seq = "&quot;";
            break;
        case '\'':
            seq = "&apos;";
            break;
        case '>':
            seq = "&gt;";
            break;
        case '&':
            seq = "&amp;";
            break;
    }

    if (seq.empty()) {
        unicode_buffer += character;
    } else {
        unicode_buffer += seq;
    }
}

void dump_tag(Glib::ustring& unicode_buffer, const Glib::ustring& tag_name,
              bool opening) {
    // By convention, TextBuffer::Tag name may come in the format
    // "tag_name|tag_value". This format should be dumped as <tag_name
    // value="tag_value">.
    std::string name, value;
    size_t delim = tag_name.find('|');
    if (delim != std::string::npos) {
        name = tag_name.substr(0, delim);
        value = tag_name.substr(delim + 1);
    } else {
        name = tag_name;
    }

    unicode_buffer.push_back('<');
    if (!opening) {
        unicode_buffer.push_back('/');
    }
    unicode_buffer += name;
    if (opening && !value.empty()) {
        unicode_buffer += " value=\"" + value + '\"';
    }
    unicode_buffer.push_back('>');
}

guint8* ff_xml_serialize(const Glib::RefPtr<Gtk::TextBuffer>& content_buffer,
                         const Gtk::TextBuffer::iterator& start,
                         const Gtk::TextBuffer::iterator& end, gsize& length) {
    Glib::ustring unicode_buffer;

    // Gtk::TextBuffer doesn't enforce nested ranges, so the sequence
    // "aa<bold>bc<italic>dd</bold>efg</italic>hi" is perfectly valid. We will
    // use the tag stack to normalize opening and closing tags to follow XML
    // convention.
    std::stack<std::string> open_tags;

    dump_tag(unicode_buffer, "ff_root", true);

    for (auto it = start; it != end; ++it) {
        // Retrieve closing tags
        std::vector<Glib::RefPtr<Gtk::TextTag>> closing_tags =
            it.get_toggled_tags(false);

        // Try to close the tags in the reverse order of opening
        bool closing_tag_found = true;
        std::stack<std::string> temporarily_closed_tags;

        while (!closing_tags.empty() && !open_tags.empty()) {
            const std::string& last_open_tag = open_tags.top();
            auto tag_it = std::find_if(
                closing_tags.begin(), closing_tags.end(),
                [last_open_tag](Glib::RefPtr<const Gtk::TextTag> closing_tag) {
                    return closing_tag->property_name() == last_open_tag;
                });
            if (tag_it == closing_tags.end()) {
                // Closing tag is conflicting with the open tags stack
                temporarily_closed_tags.push(last_open_tag);
            } else {
                // Closing tag correctly corresponds to the latest open tag
                closing_tags.erase(tag_it);
            }
            dump_tag(unicode_buffer, last_open_tag, false);
            open_tags.pop();
        }

        if (!closing_tags.empty()) {
            std::cerr << "TextBuffer corruption: some closing tags haven't "
                         "been opened."
                      << std::endl;
        }

        // Reopen the tags which were temporarily closed to resolve conflicts.
        while (!temporarily_closed_tags.empty()) {
            const std::string& tag_name = temporarily_closed_tags.top();
            open_tags.push(tag_name);
            dump_tag(unicode_buffer, tag_name, true);
            temporarily_closed_tags.pop();
        }

        // Dump opening tags
        std::vector<Glib::RefPtr<Gtk::TextTag>> opening_tags =
            it.get_toggled_tags(true);

        for (auto tag : opening_tags) {
            Glib::ustring tag_name = tag->property_name();
            dump_tag(unicode_buffer, tag_name, true);
            open_tags.push(tag_name);
        }

        dump_character(unicode_buffer, *it);
    }

    // Dump the remaining closing tags
    while (!open_tags.empty()) {
        dump_tag(unicode_buffer, open_tags.top(), false);
        open_tags.pop();
    }

    dump_tag(unicode_buffer, "ff_root", false);

    length = unicode_buffer.bytes();
    char* utf8_buffer = new char[length + 1];
    std::strcpy(utf8_buffer, unicode_buffer.c_str());

    return (guint8*)utf8_buffer;
}

const std::string RichTechEditor::rich_text_mime_type =
    "application/vnd.fontforge.rich-text+xml";

RichTechEditor::RichTechEditor() {
    auto bold_tag = text_view_.get_buffer()->create_tag("bold");
    bold_tag->property_weight() = 700;

    ToggleTagButton* bold_button =
        Gtk::make_managed<ToggleTagButton>(text_view_.get_buffer(), bold_tag);
    bold_button->set_icon_name("format-text-bold");

    auto italic_tag = text_view_.get_buffer()->create_tag("italic");
    italic_tag->property_style() = Pango::STYLE_ITALIC;

    ToggleTagButton* italic_button =
        Gtk::make_managed<ToggleTagButton>(text_view_.get_buffer(), italic_tag);
    italic_button->set_icon_name("format-text-italic");

    TagComboBox* stretch_combo =
        Gtk::make_managed<TagComboBox>(text_view_.get_buffer());

    toolbar_.append(*bold_button);
    toolbar_.append(*italic_button);
    toolbar_.append(*stretch_combo);

    toolbar_.set_hexpand();

    text_view_.set_wrap_mode(Gtk::WRAP_WORD);
    text_view_.set_hexpand();
    text_view_.set_vexpand();

    auto result = text_view_.get_buffer()->register_serialize_format(
        rich_text_mime_type, &ff_xml_serialize);

    scrolled_.add(text_view_);
    attach(toolbar_, 0, 0);
    attach(scrolled_, 0, 1);
}

RichTechEditor::ToggleTagButton::ToggleTagButton(
    Glib::RefPtr<Gtk::TextBuffer> text_buffer, Glib::RefPtr<Gtk::TextTag> tag)
    : text_buffer_(text_buffer), tag_(tag) {
    // Called whenever the selection or the cursor position is changed. Sets the
    // correct visual state of the widget
    text_buffer_->signal_mark_set().connect(
        sigc::mem_fun(*this, &ToggleTagButton::on_buffer_cursor_changed));

    // Called whenever a character is typed into the buffer. Set the tag on this
    // character according to the widget state.
    text_buffer_->signal_insert().connect(
        [this](const Gtk::TextBuffer::iterator& pos, const Glib::ustring& text,
               int bytes) {
            Gtk::TextBuffer::iterator start = pos;
            if (start.backward_chars(text.size())) {
                toggle_tag(start, pos);
            }
        });
}

void RichTechEditor::ToggleTagButton::toggle_tag(
    const Gtk::TextBuffer::iterator& start,
    const Gtk::TextBuffer::iterator& end) {
    if (get_active()) {
        text_buffer_->apply_tag(tag_, start, end);
    } else {
        text_buffer_->remove_tag(tag_, start, end);
    }
}

void RichTechEditor::ToggleTagButton::on_button_toggled() {
    Gtk::TextBuffer::iterator start, end;
    if (text_buffer_->get_selection_bounds(start, end)) {
        toggle_tag(start, end);
    }
}

void RichTechEditor::ToggleTagButton::on_buffer_cursor_changed(
    const Gtk::TextBuffer::iterator&,
    const Glib::RefPtr<Gtk::TextBuffer::Mark>& mark) {
    if (mark->get_name() != "insert") {
        return;
    }

    Gtk::TextBuffer::iterator start, end;
    bool button_active = false;

    if (!text_buffer_->get_selection_bounds(start, end)) {
        start--;
    }

    if (start.has_tag(tag_) && start.forward_to_tag_toggle(tag_)) {
        if (start >= end) {
            button_active = true;
        }
    }

    gtk_set_widget_state_without_event(
        (Gtk::ToggleToolButton*)this, &ToggleTagButton::signal_toggled,
        sigc::mem_fun(*this, &ToggleTagButton::on_button_toggled),
        [this, button_active]() { set_active(button_active); });
}

RichTechEditor::TagComboBox::TagComboBox(
    Glib::RefPtr<Gtk::TextBuffer> text_buffer)
    : text_buffer_(text_buffer) {
    // Lazy initialize statics
    if (default_id_.empty()) {
        default_id_ = "width|medium";
    }
    if (property_vec_.empty()) {
        // By convention, TextBuffer::Tag with name e.g. "width|condensed" will
        // be exported to XML tag as <width value="condensed">. Unlike in XML,
        // TextBuffer tags must have unique names.
        property_vec_ = {
            {"width|ultra-condensed", _("Ultra-Condensed (50%)"),
             Pango::STRETCH_ULTRA_CONDENSED},
            {"width|extra-condensed", _("Extra-Condensed (62.5%)"),
             Pango::STRETCH_EXTRA_CONDENSED},
            {"width|condensed", _("Condensed (75%)"), Pango::STRETCH_CONDENSED},
            {"width|semi-condensed", _("Semi-Condensed (87.5%)"),
             Pango::STRETCH_SEMI_CONDENSED},
            {"width|medium", _("Medium (100%)"), Pango::STRETCH_NORMAL},
            {"width|semi-expanded", _("Semi-Expanded (112.5%)"),
             Pango::STRETCH_SEMI_EXPANDED},
            {"width|expanded", _("Expanded (125%)"), Pango::STRETCH_EXPANDED},
            {"width|extra-expanded", _("Extra-Expanded (150%)"),
             Pango::STRETCH_EXTRA_EXPANDED},
            {"width|ultra-expanded", _("Ultra-Expanded (200%)"),
             Pango::STRETCH_ULTRA_EXPANDED},
        };
    }

    for (const auto& [tag_id, label, property] : property_vec_) {
        // Create and register tag
        if (tag_id != default_id_) {
            auto tag = text_buffer_->create_tag(tag_id);
            tag->property_stretch() = property;
            tag_map_[tag_id] = tag;
        }

        // Add entry to combo box
        combo_box_.append(tag_id, label);
    }

    add(combo_box_);

    combo_box_.signal_changed().connect(
        sigc::mem_fun(*this, &TagComboBox::on_box_changed));
}

void RichTechEditor::TagComboBox::toggle_tag(
    const Gtk::TextBuffer::iterator& start,
    const Gtk::TextBuffer::iterator& end) {
    // Remove all other tags from this group, except the new one.
    for (const auto& [tag_id, tag] : tag_map_) {
        if (tag_id != default_id_ && tag_id != combo_box_.get_active_id()) {
            text_buffer_->remove_tag(tag, start, end);
        } else if (tag_id == combo_box_.get_active_id()) {
            text_buffer_->apply_tag(tag, start, end);
        }
    }
}

void RichTechEditor::TagComboBox::on_box_changed() {
    Gtk::TextBuffer::iterator start, end;
    if (text_buffer_->get_selection_bounds(start, end)) {
        toggle_tag(start, end);
    }
}

}  // namespace ff::widget
