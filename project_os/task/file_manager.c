#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

GtkWidget *output_view;

void update_output(const char *text) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_buffer_set_text(buffer, text, -1);
}

void run_command_and_update_output(const char *cmd) {
    FILE *fp;
    char path[1024];
    char buffer[8192] = "";

    fp = popen(cmd, "r");
    if (fp == NULL) {
        update_output("Error: Failed to run command.");
        return;
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        strcat(buffer, path);
    }

    pclose(fp);
    update_output(buffer);
}

void list_files(GtkWidget *widget, gpointer data) {
   
    run_command_and_update_output("ls -1");
}

void change_permissions(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Change Permissions",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "OK", GTK_RESPONSE_OK,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *entry_file = gtk_entry_new();
    GtkWidget *entry_mode = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_file), "File name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_mode), "Permissions (e.g. 755)");

    gtk_grid_attach(GTK_GRID(grid), entry_file, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_mode, 0, 1, 1, 1);

    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *filename = gtk_entry_get_text(GTK_ENTRY(entry_file));
        const char *mode_str = gtk_entry_get_text(GTK_ENTRY(entry_mode));
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "chmod %s '%s' && echo 'Permissions changed successfully.' || echo 'Error changing permissions.'", mode_str, filename);
        run_command_and_update_output(cmd);
    }
    gtk_widget_destroy(dialog);
}

void create_file_or_dir(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create File/Directory",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "OK", GTK_RESPONSE_OK,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *entry_name = gtk_entry_new();
    GtkWidget *combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "File");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Directory");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_name), "Name");

    gtk_grid_attach(GTK_GRID(grid), entry_name, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 0, 1, 1, 1);

    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
        const char *type = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
        char cmd[512];

        if (g_strcmp0(type, "File") == 0) {
            snprintf(cmd, sizeof(cmd), "touch '%s' && echo 'File created successfully.' || echo 'Error creating file.'", name);
        } else {
            snprintf(cmd, sizeof(cmd), "mkdir '%s' && echo 'Directory created successfully.' || echo 'Error creating directory.'", name);
        }

        run_command_and_update_output(cmd);
    }
    gtk_widget_destroy(dialog);
}

void delete_file_or_dir(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete File/Directory",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "OK", GTK_RESPONSE_OK,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter name to delete");
    gtk_container_add(GTK_CONTAINER(content), entry);

    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(entry));
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "rm -r '%s' && echo 'Deleted successfully.' || echo 'Error deleting file/directory.'", name);
        run_command_and_update_output(cmd);
    }
    gtk_widget_destroy(dialog);
}

void create_symlink(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create Symbolic Link",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "OK", GTK_RESPONSE_OK,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *entry_target = gtk_entry_new();
    GtkWidget *entry_link = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_target), "Target file");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_link), "Link name");

    gtk_grid_attach(GTK_GRID(grid), entry_target, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_link, 0, 1, 1, 1);

    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *target = gtk_entry_get_text(GTK_ENTRY(entry_target));
        const char *linkname = gtk_entry_get_text(GTK_ENTRY(entry_link));
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "ln -s '%s' '%s' && echo 'Symbolic link created successfully.' || echo 'Error creating symbolic link.'", target, linkname);
        run_command_and_update_output(cmd);
    }
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Manager GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 5);

    GtkWidget *btn_list = gtk_button_new_with_label("List Files");
    g_signal_connect(btn_list, "clicked", G_CALLBACK(list_files), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), btn_list, TRUE, TRUE, 0);

    GtkWidget *btn_perm = gtk_button_new_with_label("Change Permissions");
    g_signal_connect(btn_perm, "clicked", G_CALLBACK(change_permissions), window);
    gtk_box_pack_start(GTK_BOX(button_box), btn_perm, TRUE, TRUE, 0);

    GtkWidget *btn_create = gtk_button_new_with_label("Create File/Dir");
    g_signal_connect(btn_create, "clicked", G_CALLBACK(create_file_or_dir), window);
    gtk_box_pack_start(GTK_BOX(button_box), btn_create, TRUE, TRUE, 0);

    GtkWidget *btn_delete = gtk_button_new_with_label("Delete");
    g_signal_connect(btn_delete, "clicked", G_CALLBACK(delete_file_or_dir), window);
    gtk_box_pack_start(GTK_BOX(button_box), btn_delete, TRUE, TRUE, 0);

    GtkWidget *btn_symlink = gtk_button_new_with_label("Create Symlink");
    g_signal_connect(btn_symlink, "clicked", G_CALLBACK(create_symlink), window);
    gtk_box_pack_start(GTK_BOX(button_box), btn_symlink, TRUE, TRUE, 0);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), output_view, TRUE, TRUE, 5);

    GtkWidget *btn_exit = gtk_button_new_with_label("Exit");
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), btn_exit, FALSE, FALSE, 5);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

