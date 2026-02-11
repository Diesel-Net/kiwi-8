#include "file_dialog.h"
#include <gtk/gtk.h>
#include <cstring>

std::vector<std::string> open_file_dialog(const std::string &title, const std::string &defaultDir, const std::vector<std::string> &filterExtensions) {
    std::vector<std::string> result;

    // Initialize GTK if not already initialized
    if (!gtk_init_check(nullptr, nullptr)) {
        return result;
    }

    // Create file chooser dialog
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        title.c_str(),
        nullptr,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        nullptr
    );

    // Set default directory if provided
    if (!defaultDir.empty() && defaultDir != "~") {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), defaultDir.c_str());
    }

    // Add file filters for supported extensions
    if (!filterExtensions.empty()) {
        // Create a filter for supported extensions
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "Chip-8 ROMs");

        for (const auto &ext : filterExtensions) {
            if (!ext.empty()) {
                std::string pattern = "*." + ext;
                gtk_file_filter_add_pattern(filter, pattern.c_str());
            }
        }

        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

        // Add "All Files" filter
        GtkFileFilter *allFilter = gtk_file_filter_new();
        gtk_file_filter_set_name(allFilter, "All Files");
        gtk_file_filter_add_pattern(allFilter, "*");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), allFilter);
    }

    // Show dialog and get response
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            result.push_back(std::string(filename));
            g_free(filename);
        }
    }

    // Hide dialog and process pending events before destroying
    gtk_widget_hide(dialog);
    while (gtk_events_pending()) gtk_main_iteration();
    gtk_widget_destroy(dialog);

    return result;
}
